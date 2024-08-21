/**
 * SPDX-FileCopyrightText: (C) 2006 Dominik Seichter <domseichter@web.de>
 * SPDX-FileCopyrightText: (C) 2020 Francesco Pretto <ceztko@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PDF_STRING_H
#define PDF_STRING_H

#include "PdfDeclarations.h"

#include "PdfDataProvider.h"

namespace PoDoFo {

/** A string that can be written to a PDF document.
 *  If it contains binary data it is automatically
 *  converted into a hex string, otherwise a normal PDF
 *  string is written to the document.
 *
 *  PdfString is an implicitly shared class. As a reason
 *  it is very fast to copy PdfString objects.
 *
 */
class PODOFO_API PdfString final : public PdfDataProvider
{
public:
    /** Create an empty string
     */
    PdfString();

    PdfString(charbuff&& buff, bool isHex);

    template<std::size_t N>
    PdfString(const char(&str)[N])
        : m_isHex(false)
    {
        initFromUtf8String(str, N - 1, true);
    }

    template<typename T, typename = std::enable_if_t<std::is_same_v<T, const char*>>>
    PdfString(T str)
        : m_isHex(false)
    {
        initFromUtf8String(str, std::char_traits<char>::length(str), false);
    }

    /** Construct a new PdfString from a utf-8 string
     *  The input string will be copied.
     *
     *  \param str the string to copy
     */
    PdfString(const std::string_view& view);
    PdfString(const std::string& str);

    /** Construct a new PdfString from a utf-8 string
     *  \param str the string to move from
     */
    PdfString(std::string&& str);

    /** Copy an existing PdfString
     *  \param rhs another PdfString to copy
     */
    PdfString(const PdfString& rhs) = default;

    /** Construct a new PdfString from an utf-8 encoded string.
     *
     *  \param view a buffer
     *  \param hex true if the string should be written as hex string
     */
    static PdfString FromRaw(const bufferview& view, bool hex = true);

    /** Set hex-encoded data as the strings data.
     *  \param hexView must be hex-encoded data.
     *  \param len   length of the hex-encoded data.
     *  \param encrypt if !nullptr, assume the hex data is encrypted and should be decrypted after hex-decoding.
     */
    static PdfString FromHexData(const std::string_view& hexView, const PdfStatefulEncrypt* encrypt = { });

    /** Check if this is a hex string.
     *
     *  If true the data will be hex-encoded when the string is written to
     *  a PDF file.
     *
     *  \returns true if this is a hex string.
     *  \see GetString() will return the raw string contents (not hex-encoded)
     */
    inline bool IsHex() const { return m_isHex; }

    /**
     * A PdfString can be an unevaluated raw buffer, or
     * can be a Ascii, PdfDocEncoding or Unicode string
     */
    PdfStringCharset GetCharset() const;

    bool IsEmpty() const;

    /**
     * True if the raw data buffer has been evaluated to a string
     */
    bool IsStringEvaluated() const;

    /** The contents of the string as UTF-8 string.
     *
     *  The string's contents are always returned as
     *  UTF-8 by this function. Works for Unicode strings
     *  and for non-Unicode strings.
     *
     *  This is the preferred way to access the string's contents.
     *
     *  \returns the string's contents always as UTF-8
     */
    std::string_view GetString() const;

    std::string_view GetRawData() const;

    void Write(OutputStream& stream, PdfWriteFlags writeMode,
        const PdfStatefulEncrypt* encrypt, charbuff& buffer) const override;

    /** Copy an existing PdfString
     *  \param rhs another PdfString to copy
     *  \returns this object
     */
    PdfString& operator=(const PdfString& rhs) = default;

    /** Comparison operator
     *
     *  UTF-8 and strings of the same data compare equal. Whether
     *  the string will be written out as hex is not considered - only the real "text"
     *  is tested for equality.
     *
     *  \param rhs compare to this string object
     *  \returns true if both strings have the same contents
     */
    bool operator==(const PdfString& rhs) const;
    bool operator==(const char* str) const;
    bool operator==(const std::string& str) const;
    bool operator==(const std::string_view& view) const;

    /** Comparison operator
     *  \param rhs compare to this string object
     *  \returns true if strings have different contents
     */
    bool operator!=(const PdfString& rhs) const;
    bool operator!=(const char* str) const;
    bool operator!=(const std::string& str) const;
    bool operator!=(const std::string_view& view) const;

    /** Default cast to utf8 string view
     */
    operator std::string_view() const;

private:
    // Delete constructor with nullptr
    PdfString(std::nullptr_t) = delete;

    /** Construct a new PdfString from a 0-terminated string.
     *
     *  The input string will be copied.
     *  if m_Hex is true the copied data will be hex-encoded.
     *
     *  \param view the string to copy, must not be nullptr
     *
     */
    void initFromUtf8String(const char* str, size_t length, bool literal);
    void ensureCharsEvaluated() const;

private:
    struct StringData
    {
        StringData();

        StringData(const std::string_view& view);

        StringData(charbuff&& buff, bool stringEvaluated);

        ~StringData();

        union
        {
            charbuff Chars;
            std::string_view Utf8View;
        };
        const bool CharsAllocated;
        bool StringEvaluated;
        PdfStringCharset CharSet;
    };

private:
    std::shared_ptr<StringData> m_data;
    bool m_isHex;    // This string is converted to hex during writing it out
};

}

#endif // PDF_STRING_H
