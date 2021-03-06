// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

//
// this file includes all PUBLIC common types.
//

namespace folly {
class exception_wrapper;
class IOBuf;

template <typename T>
class Range;
typedef Range<const char*> StringPiece;
}

namespace rsocket {

constexpr std::chrono::seconds kDefaultKeepaliveInterval{5};

constexpr int64_t kMaxRequestN = std::numeric_limits<int32_t>::max();

/// A unique identifier of a stream.
using StreamId = uint32_t;

using ResumePosition = int64_t;
constexpr const ResumePosition kUnspecifiedResumePosition = -1;

std::string hexDump(folly::StringPiece s);

/// Indicates the reason why the stream stateMachine received a terminal signal
/// from the connection.
enum class StreamCompletionSignal {
  CANCEL,
  COMPLETE,
  APPLICATION_ERROR,
  ERROR,
  INVALID_SETUP,
  UNSUPPORTED_SETUP,
  REJECTED_SETUP,
  CONNECTION_ERROR,
  CONNECTION_END,
  SOCKET_CLOSED,
};

enum class RSocketMode : uint8_t { SERVER, CLIENT };

std::ostream& operator<<(std::ostream&, RSocketMode);

enum class StreamType {
  REQUEST_RESPONSE,
  STREAM,
  CHANNEL,
  FNF,
};

enum class RequestOriginator {
  LOCAL,
  REMOTE,
};

std::string to_string(StreamCompletionSignal);
std::ostream& operator<<(std::ostream&, StreamCompletionSignal);

class StreamInterruptedException : public std::runtime_error {
 public:
  explicit StreamInterruptedException(int _terminatingSignal);
  int terminatingSignal;
};

class ResumeIdentificationToken {
 public:
  /// Creates an empty token.
  ResumeIdentificationToken();

  // The stringToken and ::str() function should complement
  // each other.  The string representation should be of the
  // format 0x44ab7cf01fd290b63140d01ee789cfb6
  explicit ResumeIdentificationToken(const std::string& stringToken);

  static ResumeIdentificationToken generateNew();

  const std::vector<uint8_t>& data() const {
    return bits_;
  }

  void set(std::vector<uint8_t> newBits);

  bool operator==(const ResumeIdentificationToken& right) const {
    return data() == right.data();
  }

  bool operator!=(const ResumeIdentificationToken& right) const {
    return data() != right.data();
  }

  bool operator<(const ResumeIdentificationToken& right) const {
    return data() < right.data();
  }

  std::string str() const;

 private:
  explicit ResumeIdentificationToken(std::vector<uint8_t> bits)
      : bits_(std::move(bits)) {}

  std::vector<uint8_t> bits_;
};

std::ostream& operator<<(std::ostream&, const ResumeIdentificationToken&);

// bug in GCC: https://bugzilla.redhat.com/show_bug.cgi?id=130601
#pragma push_macro("major")
#pragma push_macro("minor")
#undef major
#undef minor

struct ProtocolVersion {
  uint16_t major{};
  uint16_t minor{};

  constexpr ProtocolVersion() = default;
  constexpr ProtocolVersion(uint16_t _major, uint16_t _minor)
      : major(_major), minor(_minor) {}

  static const ProtocolVersion Unknown;
  static const ProtocolVersion Latest;
  static ProtocolVersion Current();
};

#pragma pop_macro("major")
#pragma pop_macro("minor")

std::ostream& operator<<(std::ostream&, const ProtocolVersion&);

constexpr inline bool operator==(
    const ProtocolVersion& left,
    const ProtocolVersion& right) {
  return left.major == right.major && left.minor == right.minor;
}

constexpr inline bool operator!=(
    const ProtocolVersion& left,
    const ProtocolVersion& right) {
  return !(left == right);
}

constexpr inline bool operator<(
    const ProtocolVersion& left,
    const ProtocolVersion& right) {
  return left != ProtocolVersion::Unknown &&
      right != ProtocolVersion::Unknown &&
      (left.major < right.major ||
       (left.major == right.major && left.minor < right.minor));
}

constexpr inline bool operator>(
    const ProtocolVersion& left,
    const ProtocolVersion& right) {
  return left != ProtocolVersion::Unknown &&
      right != ProtocolVersion::Unknown &&
      (left.major > right.major ||
       (left.major == right.major && left.minor > right.minor));
}

class FrameSink;

} // reactivesocket
