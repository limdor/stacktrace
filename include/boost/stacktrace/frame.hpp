// Copyright Antony Polukhin, 2016-2017.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STACKTRACE_FRAME_HPP
#define BOOST_STACKTRACE_FRAME_HPP

#include <boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#include <iosfwd>
#include <string>

#include <boost/core/explicit_operator_bool.hpp>

#include <boost/stacktrace/safe_dump_to.hpp> // boost::stacktrace::detail::native_frame_ptr_t

#include <boost/stacktrace/detail/push_options.pp>

namespace boost { namespace stacktrace {

class frame;

namespace detail {
    BOOST_STACKTRACE_FUNCTION std::string to_string(const frame* frames, std::size_t size);
} // namespace detail

/// Non-owning class that references the frame information stored inside the boost::stacktrace::stacktrace class.
class frame {
public:
    typedef boost::stacktrace::detail::native_frame_ptr_t native_frame_ptr_t;

private:
    /// @cond
    native_frame_ptr_t addr_;
    /// @endcond

public:
    /// @brief Constructs frame that references NULL address.
    /// Calls to source_file() and source_line() will return empty string.
    /// Calls to source_line() will return 0.
    ///
    /// @b Complexity: O(1).
    ///
    /// @b Async-Handler-Safety: Safe.
    /// @throws Nothing.
    BOOST_CONSTEXPR frame() BOOST_NOEXCEPT
        : addr_(0)
    {}

#ifdef BOOST_STACKTRACE_DOXYGEN_INVOKED
    /// @brief Copy constructs frame.
    ///
    /// @b Complexity: O(1).
    ///
    /// @b Async-Handler-Safety: Safe.
    /// @throws Nothing.
    frame(const frame&) = default;

    /// @brief Copy assigns frame.
    ///
    /// @b Complexity: O(1).
    ///
    /// @b Async-Handler-Safety: Safe.
    /// @throws Nothing.
    frame& operator=(const frame&) = default;
#endif

    /// @brief Constructs frame that references addr and could later generate information about that address using platform specific features.
    ///
    /// @b Complexity: O(1).
    ///
    /// @b Async-Handler-Safety: Safe.
    /// @throws Nothing.
    BOOST_CONSTEXPR explicit frame(native_frame_ptr_t addr) BOOST_NOEXCEPT
        : addr_(addr)
    {}

    /// @returns Name of the frame (function name in a human readable form).
    ///
    /// @b Complexity: unknown (lots of platform specific work).
    ///
    /// @b Async-Handler-Safety: Unsafe.
    /// @throws std::bad_alloc if not enough memory to construct resulting string.
    BOOST_STACKTRACE_FUNCTION std::string name() const;

    /// @returns Address of the frame function.
    ///
    /// @b Complexity: O(1).
    ///
    /// @b Async-Handler-Safety: Safe.
    /// @throws Nothing.
    BOOST_CONSTEXPR native_frame_ptr_t address() const BOOST_NOEXCEPT {
        return addr_;
    }

    /// @returns Path to the source file, were the function of the frame is defined. Returns empty string
    /// if this->source_line() == 0.
    /// @throws std::bad_alloc if not enough memory to construct resulting string.
    ///
    /// @b Complexity: unknown (lots of platform specific work).
    ///
    /// @b Async-Handler-Safety: Unsafe.
    BOOST_STACKTRACE_FUNCTION std::string source_file() const;

    /// @returns Code line in the source file, were the function of the frame is defined.
    /// @throws std::bad_alloc if not enough memory to construct string for internal needs.
    ///
    /// @b Complexity: unknown (lots of platform specific work).
    ///
    /// @b Async-Handler-Safety: Unsafe.
    BOOST_STACKTRACE_FUNCTION std::size_t source_line() const;

    /// @brief Checks that frame is not references NULL address.
    /// @returns `true` if `this->address() != 0`
    ///
    /// @b Complexity: O(1)
    ///
    /// @b Async-Handler-Safety: Safe.
    BOOST_CONSTEXPR_EXPLICIT_OPERATOR_BOOL()

    /// @brief Checks that frame references NULL address.
    /// @returns `true` if `this->address() == 0`
    ///
    /// @b Complexity: O(1)
    ///
    /// @b Async-Handler-Safety: Safe.
    BOOST_CONSTEXPR bool empty() const BOOST_NOEXCEPT { return !address(); }
    
    /// @cond
    BOOST_CONSTEXPR bool operator!() const BOOST_NOEXCEPT { return !address(); }
    /// @endcond
};

/// Comparison operators that provide platform dependant ordering and have O(1) complexity; are Async-Handler-Safe.
BOOST_CONSTEXPR inline bool operator< (const frame& lhs, const frame& rhs) BOOST_NOEXCEPT { return lhs.address() < rhs.address(); }
BOOST_CONSTEXPR inline bool operator> (const frame& lhs, const frame& rhs) BOOST_NOEXCEPT { return rhs < lhs; }
BOOST_CONSTEXPR inline bool operator<=(const frame& lhs, const frame& rhs) BOOST_NOEXCEPT { return !(lhs > rhs); }
BOOST_CONSTEXPR inline bool operator>=(const frame& lhs, const frame& rhs) BOOST_NOEXCEPT { return !(lhs < rhs); }
BOOST_CONSTEXPR inline bool operator==(const frame& lhs, const frame& rhs) BOOST_NOEXCEPT { return lhs.address() == rhs.address(); }
BOOST_CONSTEXPR inline bool operator!=(const frame& lhs, const frame& rhs) BOOST_NOEXCEPT { return !(lhs == rhs); }

/// Fast hashing support, O(1) complexity; Async-Handler-Safe.
inline std::size_t hash_value(const frame& f) BOOST_NOEXCEPT {
    return reinterpret_cast<std::size_t>(f.address());
}

/// Outputs stacktrace::frame in a human readable format to string; unsafe to use in async handlers.
BOOST_STACKTRACE_FUNCTION std::string to_string(const frame& f);

/// Outputs stacktrace::frame in a human readable format to output stream; unsafe to use in async handlers.
template <class CharT, class TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& os, const frame& f) {
    return os << boost::stacktrace::to_string(f);
}

}} // namespace boost::stacktrace

/// @cond

#include <boost/stacktrace/detail/pop_options.pp>

#ifndef BOOST_STACKTRACE_LINK
#   if defined(BOOST_STACKTRACE_USE_NOOP)
#       include <boost/stacktrace/detail/frame_noop.ipp>
#   elif defined(BOOST_MSVC) || defined(BOOST_STACKTRACE_USE_WINDBG) || defined(BOOST_STACKTRACE_USE_WINDBG_CACHED)
#       include <boost/stacktrace/detail/frame_msvc.ipp>
#   else
#       include <boost/stacktrace/detail/frame_unwind.ipp>
#   endif
#endif
/// @endcond


#endif // BOOST_STACKTRACE_FRAME_HPP
