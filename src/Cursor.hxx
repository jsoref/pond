/*
 * author: Max Kellermann <mk@cm4all.com>
 */

#pragma once

#include "util/BindMethod.hxx"

#include <boost/intrusive/list_hook.hpp>

#include <assert.h>

class Database;
class Record;

class Cursor final
	: public boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>> {

	Database &database;
	const Record *next = nullptr;

	BoundMethod<void()> append_callback;

public:
	Cursor(Database &_database, BoundMethod<void()> _append_callback)
		:database(_database), append_callback(_append_callback) {}

	void Rewind();

	void Follow();

	void OnAppend(const Record &record) {
		assert(!is_linked());
		assert(next == nullptr);

		next = &record;
		append_callback();
	}

	void clear() {
		unlink();
		next = nullptr;
	}

	operator bool() const {
		return next != nullptr;
	}

	const Record &operator*() const {
		assert(next != nullptr);
		assert(!is_linked());

		return *next;
	}

	const Record *operator->() const {
		assert(next != nullptr);
		assert(!is_linked());

		return next;
	}

	Cursor &operator++();
};