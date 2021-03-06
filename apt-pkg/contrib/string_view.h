/*
 * Basic implementation of string_view
 *
 * (C) 2015 Julian Andres Klode <jak@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#if !defined(APT_STRINGVIEW_H) && defined(APT_PKG_EXPOSE_STRING_VIEW)
#define APT_STRINGVIEW_H
#include <string.h>
#include <string>

namespace APT {

/**
 * \brief Simple subset of std::string_view from C++17
 *
 * This is an internal implementation of the subset of std::string_view
 * used by APT. It is not meant to be used in programs, only inside the
 * library for performance critical paths.
 */
class StringView {
    const char *data_;
    size_t size_;

public:
    static constexpr size_t npos = static_cast<size_t>(-1);

    /* Constructors */
    constexpr StringView() : data_(""), size_(0) {}
    constexpr StringView(const char *data, size_t size) : data_(data), size_(size) {}

    StringView(const char *data) : data_(data), size_(strlen(data)) {}
    StringView(std::string const & str): data_(str.data()), size_(str.size()) {}


    /* Viewers */
    constexpr StringView substr(size_t pos, size_t n = npos) const {
        return StringView(data_ + pos, n > (size_ - pos) ? (size_ - pos) : n);
    }

    size_t find(int c, size_t pos=0) const {
        if (pos != 0)
            return substr(pos).find(c);

        const char *found = static_cast<const char*>(memchr(data_, c, size_));

        if (found == NULL)
            return npos;

        return found - data_;
    }

    size_t rfind(int c, size_t pos=npos) const {
        if (pos != npos)
            return substr(0, pos).rfind(c);

        const char *found = static_cast<const char*>(memrchr(data_, c, size_));

        if (found == NULL)
            return npos;

        return found - data_;
    }

    /* Conversions */
    std::string to_string() const {
        return std::string(data_, size_);
    }

    /* Comparisons */
    int compare(size_t pos, size_t n, StringView other) const {
        return substr(pos, n).compare(other);
    }

    int compare(StringView other) const {
        int res;

        res = memcmp(data_, other.data_, std::min(size_, other.size_));
        if (res != 0)
            return res;
        if (size_ == other.size_)
            return res;

        return (size_ > other.size_) ? 1 : -1;
    }

    /* Optimization: If size not equal, string cannot be equal */
    bool operator ==(StringView other) const { return size_ == other.size_ && compare(other) == 0; }
    bool operator !=(StringView other) const { return !(*this == other); }

    /* Accessors */
    constexpr bool empty() const { return size_ == 0; }
    constexpr const char* data() const { return data_; }
    constexpr const char* begin() const { return data_; }
    constexpr const char* end() const { return data_ + size_; }
    constexpr char operator [](size_t i) const { return data_[i]; }
    constexpr size_t size() const { return size_; }
    constexpr size_t length() const { return size_; }
};


}

inline bool operator ==(const char *other, APT::StringView that);
inline bool operator ==(const char *other, APT::StringView that) { return that.operator==(other); }

#endif
