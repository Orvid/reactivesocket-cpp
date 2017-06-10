#pragma once

#include <folly/ExceptionWrapper.h>
#include <condition_variable>
#include <mutex>
#include <vector>
#include "src/Payload.h"
#include "yarpl/Refcounted.h"

namespace rsocket {
namespace tck {

class BaseSubscriber : public virtual yarpl::Refcounted {
 public:
  virtual void request(int n) = 0;
  virtual void cancel() = 0;
  virtual void awaitTerminalEvent();
  virtual void awaitAtLeast(int numItems);
  virtual void awaitNoEvents(int waitTime);
  virtual void assertNoErrors();
  virtual void assertError();
  virtual void assertValues(
      const std::vector<std::pair<std::string, std::string>>& values);
  virtual void assertValueCount(size_t valueCount);
  virtual void assertReceivedAtLeast(size_t valueCount);
  virtual void assertCompleted();
  virtual void assertNotCompleted();
  virtual void assertCanceled();

 protected:
  std::atomic<bool> canceled_{false};

  ////////////////////////////////////////////////////////////////////////////
  std::mutex mutex_; // all variables below has to be protected with the mutex

  std::vector<std::pair<std::string, std::string>> values_;
  std::condition_variable valuesCV_;
  std::atomic<int> valuesCount_{0};

  std::vector<std::exception_ptr> errors_;

  std::condition_variable terminatedCV_;
  std::atomic<bool> completed_{false}; // by onComplete
  std::atomic<bool> errored_{false}; // by onError
  ////////////////////////////////////////////////////////////////////////////
};

} // tck
} // reactivesocket