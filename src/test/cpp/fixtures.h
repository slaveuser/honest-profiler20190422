#include "test.h"
#include "../../main/cpp/circular_queue.h"

#ifndef FIXTURES_H
#define FIXTURES_H

class ItemHolder : public QueueListener {
public:
  explicit ItemHolder() {}

  void record(const JVMPI_CallTrace &trace, ThreadBucketPtr info) {
    timespec spec;
    TimeUtils::current_utc_time(&spec);

    record(spec, trace, std::move(info));
  }

  virtual void record(const timespec &ts, const JVMPI_CallTrace &trace, ThreadBucketPtr info) {
    CHECK_EQUAL(2, trace.num_frames);
    CHECK_EQUAL((JNIEnv *)envId, trace.env_id);

    JVMPI_CallFrame frame0 = trace.frames[0];
    CHECK_EQUAL(52, frame0.lineno);
    CHECK_EQUAL((jmethodID)1, frame0.method_id);
  }

  long envId;
};

// Queue too big to stack allocate,
// So we use a fixture
struct GivenQueue {
  GivenQueue() : holder(), queue(holder, DEFAULT_MAX_FRAMES_TO_CAPTURE) {
  }

  ~GivenQueue() {}

  ItemHolder holder;
  CircularQueue queue;

  // wrap an easy to test api around the queue
  bool pop(const long envId) {
    holder.envId = envId;
    return queue.pop();
  }
};

#endif /* FIXTURES_H */
