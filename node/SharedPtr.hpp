/*
 * ZeroTier One - Global Peer to Peer Ethernet
 * Copyright (C) 2012-2013  ZeroTier Networks LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * --
 *
 * ZeroTier may be used and distributed under the terms of the GPLv3, which
 * are available at: http://www.gnu.org/licenses/gpl-3.0.html
 *
 * If you would like to embed ZeroTier into a commercial application or
 * redistribute it in a modified binary form, please contact ZeroTier Networks
 * LLC. Start here: http://www.zerotier.com/
 */

#ifndef _ZT_SHAREDPTR_HPP
#define _ZT_SHAREDPTR_HPP

#include "Mutex.hpp"
#include "AtomicCounter.hpp"

namespace ZeroTier {

/**
 * Simple reference counted pointer
 *
 * This is an introspective shared pointer. Classes that need to be reference
 * counted must list this as a 'friend' and must have a private instance of
 * AtomicCounter called __refCount. They should also have private destructors,
 * since only this class should delete them.
 *
 * Because this is introspective, it is safe to apply to a naked pointer
 * multiple times provided there is always at least one holding SharedPtr.
 */
template<typename T>
class SharedPtr
{
public:
	SharedPtr()
		throw() :
		_ptr((T *)0)
	{
	}

	SharedPtr(T *obj)
		throw() :
		_ptr(obj)
	{
		++obj->__refCount;
	}

	SharedPtr(const SharedPtr &sp)
		throw() :
		_ptr(sp._getAndInc())
	{
	}

	~SharedPtr()
	{
		if (_ptr) {
			if (--_ptr->__refCount <= 0)
				delete _ptr;
		}
	}

	inline SharedPtr &operator=(const SharedPtr &sp)
	{
		if (_ptr != sp._ptr) {
			T *p = sp._getAndInc();
			if (_ptr) {
				if (--_ptr->__refCount <= 0)
					delete _ptr;
			}
			_ptr = p;
		}
		return *this;
	}

	inline operator bool() const throw() { return (_ptr); }
	inline T &operator*() const throw() { return *_ptr; }
	inline T *operator->() const throw() { return _ptr; }

	/**
	 * @return Raw pointer to held object
	 */
	inline T *ptr() const throw() { return _ptr; }

	/**
	 * Set this pointer to null
	 */
	inline void zero()
	{
		if (_ptr) {
			if (--_ptr->__refCount <= 0)
				delete _ptr;
		}
		_ptr = (T *)0;
	}

	inline bool operator==(const SharedPtr &sp) const throw() { return (_ptr == sp._ptr); }
	inline bool operator!=(const SharedPtr &sp) const throw() { return (_ptr != sp._ptr); }
	inline bool operator>(const SharedPtr &sp) const throw() { return (_ptr > sp._ptr); }
	inline bool operator<(const SharedPtr &sp) const throw() { return (_ptr < sp._ptr); }
	inline bool operator>=(const SharedPtr &sp) const throw() { return (_ptr >= sp._ptr); }
	inline bool operator<=(const SharedPtr &sp) const throw() { return (_ptr <= sp._ptr); }

private:
	inline T *_getAndInc() const
		throw()
	{
		if (_ptr)
			++_ptr->__refCount;
		return _ptr;
	}

	T *_ptr;
};

} // namespace ZeroTier

#endif
