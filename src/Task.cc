#include "dataflow/Task.hh"

dataflow::Task::Task()
  : priority_(0), mode_(TaskMode::MULTIPLE), state_(TaskState::RUNNING) { }

dataflow::IterationResult dataflow::Task::attempt() {
  IterationResult result;

  std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
  if(mode_ == TaskMode::SINGLE) {
    // Grab the mutex, then run apply().
    // If a different thread has the mutex, bail out.
    bool has_lock = lock.try_lock();
    if(has_lock){
      result = apply();
    } else {
      result = dataflow::IterationResult::SKIP;
    }
  } else {
    // Run apply() immediately, without grabbing the mutex.
    result = apply();
  }

  if(result == IterationResult::STOP){
    state_ = TaskState::STOPPED;
  }

  return result;
}

double dataflow::Task::priority() const {
  return priority_;
}

void dataflow::Task::set_priority(double value) {
  priority_ = value;
}

dataflow::TaskMode dataflow::Task::mode() const {
  return mode_;
}

void dataflow::Task::set_mode(dataflow::TaskMode value) {
  mode_ = value;
}

dataflow::TaskState dataflow::Task::state() const {
  return state_;
}
