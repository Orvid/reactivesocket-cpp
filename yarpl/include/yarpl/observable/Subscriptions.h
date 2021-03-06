// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <atomic>
#include <functional>
#include <iostream>
#include <memory>

#include "yarpl/Refcounted.h"
#include "yarpl/observable/Subscription.h"

namespace yarpl {
namespace observable {

/**
* Implementation that gets a callback when cancellation occurs.
*/
class CallbackSubscription : public Subscription {
 public:
  explicit CallbackSubscription(std::function<void()> onCancel);
  void cancel() override;

 private:
  std::function<void()> onCancel_;
};

class Subscriptions {
 public:
  static Reference<Subscription> create(std::function<void()> onCancel);
  static Reference<Subscription> create(std::atomic_bool& cancelled);
  static Reference<Subscription> create();
};

} // observable namespace
} // yarpl namespace
