#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <chrono>

class Timer {
    typedef std::chrono::steady_clock ClockType;
    typedef std::chrono::time_point<ClockType> TP;
public:
    Timer() : running_(false) {}

    void Start() {
        start_time_ = ClockType::now();
        running_ = true;
    }

    void Stop() {
        end_time_ = ClockType::now();
        running_ = false;
    }

    double ElapsedMilliseconds() {
        TP end_time = running_ ? ClockType::now() : end_time_;
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_).count();
    }

    double ElapsedSeconds() {
        return ElapsedMilliseconds() / 1000.;
    }

private:
    TP start_time_;
    TP end_time_;
    bool running_;
};

#endif // TIMER_HPP_