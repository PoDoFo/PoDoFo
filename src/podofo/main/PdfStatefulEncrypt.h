/**
 * SPDX-FileCopyrightText: (C) 2021 Francesco Pretto <ceztko@gmail.com>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 * SPDX-License-Identifier: MPL-2.0
 */

#ifndef PDF_STATEFUL_ENCRYPT_H
#define PDF_STATEFUL_ENCRYPT_H

#include "PdfDeclarations.h"
#include "PdfReference.h"

namespace PoDoFo
{
    class PdfEncrypt;

    class PODOFO_API PdfStatefulEncrypt final
    {
    public:
        PdfStatefulEncrypt(const PdfEncrypt& encrypt, const PdfReference& objref);

    public:
        /** Encrypt a character span
         */
        void EncryptTo(charbuff& out, const bufferview& view) const;

        /** Decrypt a character span
         */
        void DecryptTo(charbuff& out, const bufferview& view) const;

        size_t CalculateStreamLength(size_t length) const;

    private:
        PdfStatefulEncrypt(const PdfStatefulEncrypt&) = delete;
        PdfStatefulEncrypt& operator=(const PdfStatefulEncrypt&) = delete;

    private:
        const PdfEncrypt* m_encrypt;
        PdfReference m_currReference;       // Reference of the current PdfObject
    };
}

#endif // PDF_STATEFUL_ENCRYPT_H
