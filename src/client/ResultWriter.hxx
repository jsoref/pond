/*
 * Copyright 2017-2019 Content Management AG
 * All rights reserved.
 *
 * author: Max Kellermann <mk@cm4all.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "io/UniqueFileDescriptor.hxx"
#include "io/FdOutputStream.hxx"
#include "net/SocketDescriptor.hxx"

#include <memory>

template<typename t> struct ConstBuffer;
namespace Net::Log { struct Datagram; }

class ResultWriter {
	FileDescriptor fd;
	SocketDescriptor socket;

	std::unique_ptr<FdOutputStream> output_stream;

	/**
	 * Inside this directory, a file will be appended to for each
	 * site.
	 */
	const UniqueFileDescriptor per_site_append;

	char last_site[256];
	UniqueFileDescriptor per_site_fd;

	const bool raw, anonymize, single_site;

	size_t buffer_fill = 0;
	char buffer[65536];

public:
	ResultWriter(bool _raw, bool _anonymize, bool _single_site,
		     const char *const _per_site_append) noexcept;

	ResultWriter(const ResultWriter &) = delete;
	ResultWriter &operator=(const ResultWriter &) = delete;

	FileDescriptor GetFileDescriptor() const noexcept {
		return fd;
	}

	bool IsEmpty() const noexcept {
		return buffer_fill == 0;
	}

	void Write(ConstBuffer<void> payload);

	void Flush();

private:
	void Append(const Net::Log::Datagram &d, bool site);
};
