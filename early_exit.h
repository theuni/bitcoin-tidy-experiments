// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_EARLY_EXIT_H
#define BITCOIN_EARLY_EXIT_H

#include <cassert>
#include <variant>

enum class FatalError
{
    UNKNOWN,
    BLOCK_DISCONNECT_ERROR,
    BLOCK_MUTATED,
    BLOCK_FLUSH_FAILED,
    BLOCK_READ_FAILED,
    BLOCK_READ_CORRUPT,
    BLOCK_WRITE_FAILED,
    BLOCK_INDEX_WRITE_FAILED,
    COINSDB_WRITE_FAILED,
    DISK_SPACE_ERROR,
    FLUSH_SYSTEM_ERROR,
    UNDO_FLUSH_FAILED,
    UNDO_WRITE_FAILED
};

enum class VoidType{};

enum class UserInterrupted
{
    UNKNOWN,
    BLOCK_IMPORT_COMPLETE,
};

using EarlyExit = std::variant<std::monostate, FatalError, UserInterrupted>;

template <typename T>
class MaybeEarlyExit;

template <typename T>
EarlyExit BubbleUp(MaybeEarlyExit<T>&& ret);

template <typename T = VoidType>
class [[nodiscard]] MaybeEarlyExit : std::variant<T, FatalError, UserInterrupted>
{
    EarlyExit Bubble() const &&
    {
        if (std::holds_alternative<FatalError>(*this)) {
            return std::get<FatalError>(*this);
        } else if (std::holds_alternative<UserInterrupted>(*this)) {
            return std::get<UserInterrupted>(*this);
        } else {
            return FatalError::UNKNOWN;
        }
    }
    friend EarlyExit BubbleUp<T>(MaybeEarlyExit<T>&&);

public:
    using underlying = std::variant<T, FatalError, UserInterrupted>;
    using std::variant<T, FatalError, UserInterrupted>::variant;

    // No copying or moving, only bubbling up.
    MaybeEarlyExit& operator=(const MaybeEarlyExit&) = delete;
    MaybeEarlyExit(const MaybeEarlyExit&) = delete;
    MaybeEarlyExit& operator=(MaybeEarlyExit&&) = delete;
    MaybeEarlyExit(MaybeEarlyExit&&) = delete;

    // Set to FatalError by default so this works even when T can't be default-constructed
    MaybeEarlyExit(EarlyExit err) : std::variant<T, FatalError, UserInterrupted>(FatalError::UNKNOWN)
    {
        if (std::holds_alternative<FatalError>(err)) {
            underlying::template emplace<FatalError>(std::get<FatalError>(err));
        } else if (std::holds_alternative<UserInterrupted>(err)) {
            underlying::template emplace<UserInterrupted>(std::get<UserInterrupted>(err));
        }
    }

    bool ShouldEarlyExit() const
    {
        return !std::holds_alternative<T>(*this);
    }

    const T& operator*() const {
        assert(!ShouldEarlyExit());
        return std::get<T>(*this);
    }

    T& operator*() {
        assert(!ShouldEarlyExit());
        return std::get<T>(*this);
    }

    // Only intended for use by top-level callers to report errors
    EarlyExit GetEarlyExit() const
    {
        if (std::holds_alternative<UserInterrupted>(*this)) {
            return std::get<UserInterrupted>(*this);
        }
        if (std::holds_alternative<FatalError>(*this)) {
            return std::get<FatalError>(*this);
        }
        return std::monostate{};
    }

    // Semantics similar to std::map::try_emplace
    // Only moves the value out if it exists, otherwise assume the caller
    // will bubble it up.
    bool TryMoveOut(T& val) const
    {
        if (ShouldEarlyExit()) {
            return false;
        }
        val = std::move(std::get<T>(*this));
        return true;
    }

    // To remove after transform
    [[deprecated]] operator T() const
    {
        assert(!ShouldEarlyExit());
        return std::get<T>(*this);
    }
};

// User function to walk up the call-stack
template <typename T>
EarlyExit BubbleUp(MaybeEarlyExit<T>&& ret)
{
    return std::move(ret).Bubble();
}

#ifndef PASTE
#define PASTE(x, y) x ## y
#endif
#ifndef PASTE2
#define PASTE2(x, y) PASTE(x, y)
#endif

#define BUBBLE_UP(func) BubbleUp(func)
#define MAYBE_EXIT(func) if(auto tmp_int_ret = func; tmp_int_ret.ShouldEarlyExit()) return BubbleUp(std::move(tmp_int_ret));
#define EXIT_OR_ASSIGN(ret_val, func) if (auto tmp_int_ret = func; !tmp_int_ret.TryMoveOut(ret_val)) return BubbleUp(std::move(tmp_int_ret));
#define EXIT_OR_IF(func) if(auto tmp_int_ret = func; tmp_int_ret.ShouldEarlyExit()) return BubbleUp(std::move(tmp_int_ret)); else if (*tmp_int_ret)
#define EXIT_OR_IF_NOT(func) if(auto tmp_int_ret = func; tmp_int_ret.ShouldEarlyExit()) return BubbleUp(std::move(tmp_int_ret)); else if (!*tmp_int_ret)
// Because the temporary here cannot be scoped like the others, make it per-line unique
// __COUNTER__ could potentially be used instead, but it's non-standard and confuses LTO.
#define EXIT_OR_DECL(ret_val, func) auto PASTE2(tmp_int_ret, __LINE__) = func; if(PASTE2(tmp_int_ret, __LINE__).ShouldEarlyExit()) return BubbleUp(std::move(PASTE2(tmp_int_ret, __LINE__))); ret_val = std::move(*PASTE2(tmp_int_ret, __LINE__));


// NOOP versions of the same macros for temporarily catching in top-level functions
#define NOOP_MAYBE_EXIT(func) if ([[maybe_unused]] auto tmp_int_ret = func; true) ;
#define NOOP_EXIT_OR_ASSIGN(ret_val, func) ret_val = *func;
#define NOOP_EXIT_OR_IF(func) if(auto tmp_int_ret = func; *tmp_int_ret)
#define NOOP_EXIT_OR_IF_NOT(func) if(auto tmp_int_ret = func; !*tmp_int_ret)
#define NOOP_EXIT_OR_DECL(ret_val, func) ret_val = *func

#endif // BITCOIN_EARLY_EXIT_H
