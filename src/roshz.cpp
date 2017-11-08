#include <chrono>
#include <cmath>
#include <deque>
#include <iostream>
#include <string>

#include <ros/ros.h>

#include <roshz/statistics.h>

class DummyMsg {
  public:
    static std::string md5;
    static std::string data_type;
    static std::string const &__s_getMD5Sum() { return md5; }
    static std::string const &__s_getDataType() { return data_type; }
    void deserialize(void *) {}
};

std::string DummyMsg::md5 = "*";
std::string DummyMsg::data_type = "/";

unsigned int window_size = 10000;

unsigned int message_count = 0;
std::deque<double> times;
std::chrono::system_clock::time_point last;

void msg_callback(const DummyMsg &data) {
  auto time = std::chrono::system_clock::now();
  double diff = std::chrono::duration<double>(time - last).count();
  last = time;

  message_count++;

  if (message_count < 2) {
    return;
  }

  times.push_back(diff);
  statistics::add_data_point(diff);

  if (statistics::get_n() + 1 > window_size) {
    statistics::remove_data_point(times.front());
    times.pop_front();
  }
}

unsigned int status_count = 0;
void status_callback(const ros::WallTimerEvent &event) {
  if (status_count == message_count || times.size() < 2) {
    std::cout << "No new messages." << std::endl;
    return;
  }

  status_count = message_count;

  double avg_delay = statistics::get_mean();
  double variance = statistics::get_variance();

  std::cout << "Average delay: " << avg_delay << "s (rate of " << 1.0 / avg_delay << " Hz)" << std::endl;
  std::cout << "\tmin: " << statistics::get_min() << "s max: "  << statistics::get_max() << "s std dev: " << std::sqrt(variance) << "s window: " << statistics::get_n() + 1 << std::endl;
}

int main(int argc, char **argv) {
  std::string topic_name;
  auto hints = ros::TransportHints();

  for (int i = 1; i < argc; i++) {
    if (!std::string("-w").compare(argv[i])) {
      if (i == argc - 1) {
        std::cout << "-w requires a window size." << std::endl;
        return 1;
      }
      else {
        int user_win_size = std::stoi(argv[i++ + 1]);
        if (user_win_size < 2) {
          std::cout << "Window size must be at least 2." << std::endl;
          return 1;
        }

        window_size = user_win_size;
      }
    }

    else if (!std::string("--nodelay").compare(argv[i])) {
      hints = hints.tcpNoDelay();
    }
    else if (!std::string("--udp").compare(argv[i])) {
      hints = hints.unreliable();
    }
    else {
      topic_name = argv[i];
    }
  }

  if (!topic_name.size()) {
    std::cout << "Please supply a topic name." << std::endl;
    return 1;
  }

  std::cout << "Using a window size of " << window_size << "." << std::endl;

  ros::init(argc, argv, "roshz", ros::init_options::AnonymousName);
  ros::NodeHandle nh;
  ros::Subscriber sub = nh.subscribe(topic_name, 5000, msg_callback, hints);
  ros::WallTimer timer = nh.createWallTimer(ros::WallDuration(1.0), status_callback);

  ros::spin();

  return 0;
};
