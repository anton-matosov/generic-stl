/* 
Generic STL (genericstl)
http://genericstl.sourceforge.net/
Copyright (c) 2007-2008 Anton (shikin) Matosov

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3, 29 June 2007 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA <http://fsf.org/>


Please feel free to contact me via e-mail: shikin at users.sourceforge.net
*/

#ifndef GSTL_BASIC_STRING_HEADER
#define GSTL_BASIC_STRING_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#	pragma once
#endif

#include <iosfwd>
#include <gstl/algorithm>
#include <gstl/utility>
#include <gstl/iterator>
#include <gstl/allocator>
#include <gstl/detail/char_traits.hpp>
#include <boost/bind.hpp>

#pragma warning (disable: 4100)

namespace gstl
{
	namespace detail
	{
		/*
		21 Strings library [lib.strings]
		1 This clause describes components for manipulating sequences of “characters,” where characters may be of
		any POD (3.9) type. In this clause such types are called char-like types, and objects of char-like types are
		called char-like objects or simply “characters.”
		*/

		template
			<
				class CharT,
				class Traits,
				class Alloc,
				class PtrTraits
			>
		class string_base
		{
		public:
			enum{ min_buff_size = 4 };

			typedef CharT										char_type;
			typedef Traits										char_traits_type;
			typedef typename Alloc::rebind<char_type>::other	allocator_type;


			typedef typename allocator_type::value_type			value_type;
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::const_pointer		const_pointer;
			typedef typename allocator_type::reference			reference;
			typedef typename allocator_type::const_reference	const_reference;

			typedef typename allocator_type::size_type			size_type;
			typedef typename allocator_type::difference_type	difference_type;

			typedef PtrTraits									ptr_traits;
			
			pointer			str_;
			size_type		size_;
			size_type		reserved_;
			allocator_type	alloc_;

			
			string_base( const allocator_type& alloc = allocator_type() )
				:str_( ptr_traits::null_ptr ),
				size_(0),
				reserved_(0),
				alloc_( alloc )
			{
				reserve( min_buff_size );
			}

			void set_end( size_type n )
			{
				size_ = n;
				str_[size_] = char_type();
			}

			void reserve( size_type n = 0 )
			{
				if( n >= max_size() )
				{
					throw_length_error();
				}
				if( n > reserved_ )
				{
					n = floor( n, min_buff_size );
					pointer new_str = alloc_.allocate( n );
					//pointer old_str = str_;
					if( !ptr_traits::is_null( str_ ) )
					{
						char_traits_type::move( alloc_.address( new_str ), my_str(), size_ );
					}
					reset_ptr( new_str, size_, n );
				}
				//set_end( size_ );
			}


			void grow( size_type n )
			{
				reserve( n + 1 );
			}

			char_type* my_str()
			{
				return alloc_.address( str_ );
			}

			const char_type* my_str() const
			{
				return alloc_.address( str_ );
			}

			void throw_out_of_range() const
			{
				throw std::out_of_range( "invalid position" );
			}

			void throw_length_error() const
			{
				throw std::length_error( "invalid length" );
			}

			void reset_ptr( pointer new_str, size_type new_size, size_type new_reserved )
			{
				if( new_str != str_ )
				{
					pointer old_str = str_;
					str_ = new_str;
					alloc_.deallocate( old_str, reserved_ );
					reserved_ = new_reserved;
				}
				set_end( new_size );
			}

			size_type max_size() const
			{
				size_type max = alloc_.max_size();
				return max <= 1 ? 1 : max -1;
			}
		};

	}

	template
	<
		class CharT,
		class Traits = char_traits<CharT>,
		class Alloc = allocator<CharT>,
		class PtrTraits = typename Alloc::pointer_traits_type
	>
	class basic_string
		:private detail::string_base<CharT, Traits, Alloc, PtrTraits>
	{
		typedef detail::string_base<CharT, Traits, Alloc, PtrTraits> base_type;
	public:
		//////////////////////////////////////////////////////////////////////////
		// types:
		typedef typename base_type::char_type			char_type;
		typedef typename base_type::char_traits_type	char_traits_type;
		typedef typename base_type::allocator_type		allocator_type;


		typedef typename base_type::ptr_traits		ptr_traits;

		typedef typename base_type::value_type		value_type;
		typedef typename base_type::pointer			pointer;
		typedef typename base_type::const_pointer	const_pointer;
		typedef typename base_type::reference		reference;
		typedef typename base_type::const_reference	const_reference;

		typedef typename base_type::size_type		size_type;
		typedef typename base_type::difference_type	difference_type;


		//////////////////////////////////////////////////////////////////////////
		typedef char* iterator; //See 23.1
		typedef const char* const_iterator; // See 23.1
		//typedef typename base_type::pointer iterator; //See 23.1
		//typedef typename base_type::const_pointer const_iterator; // See 23.1
		typedef gstl::reverse_iterator<iterator> reverse_iterator;
		typedef gstl::reverse_iterator<const_iterator> const_reverse_iterator;
		static const size_type npos = static_cast<size_type>( -1 );
		
		// 21.3.1 construct/copy/destroy:
		/**
		@brief Constructs an object of class basic_string. The postconditions of this function are indicated
			in Table 38:
			data() a non-null char_type* that is copyable and can have 0 added to it
			size() 0
			capacity() an unspecified value
		*/
		explicit basic_string( const allocator_type& alloc = allocator_type() )
			:base_type( alloc )
		{
			
		}

		/**
			@brief 
			Requires: pos <= str.size()
			Throws: out_of_range if pos > str.size().
			Effects: Constructs an object of class basic_string and determines the effective length rlen of the
				initial string value as the smaller of n and str.size() - pos, as indicated in Table 39. In the first
				form, the Allocator value used is copied from str.get_allocator().
			Table 39:
			data() points at the first element of an
				allocated copy of rlen consecutive
				elements of the string controlled
				by str beginning at position pos
			size() rlen
			capacity() a value at least as large as size()
		*/
		basic_string(const basic_string& str)
			:base_type( str.get_allocator() )
		{
			assign( str, 0, npos );
		}

		/**
		   @brief the same as basic_string(const basic_string& str)
		*/
		basic_string(const basic_string& str, size_type pos, size_type n = npos,
			const allocator_type& alloc = allocator_type())
			:base_type( alloc )
		{
			assign( str, pos, n );
		}

		basic_string(const char_type* s,
			size_type n, const allocator_type& alloc = allocator_type())
			:base_type( alloc )
		{
			assign( s, n );
		}

		basic_string(const char_type* s, const allocator_type& alloc = allocator_type())
			:base_type( alloc )
		{
			assign( s );
		}

		basic_string(size_type n, char_type c, const allocator_type& alloc = allocator_type())
			:base_type( alloc )
		{
			assign( n, c );
		}

		/*		
		Effects: If InputIterator is an integral type, equivalent to
		basic_string(static_cast<size_type>(begin), static_cast<value_type>(end))
		Otherwise constructs a string from the values in the range [begin, end), as indicated in the Sequence
		Requirements table (see 23.1.1).
		*/
		template<class InputIterator>
		basic_string(InputIterator first, InputIterator last,
			const allocator_type& alloc = allocator_type())
			:base_type( alloc )
		{
			assign( first, last );
		}

		~basic_string()
		{
			alloc_.deallocate( str_, reserved_ );
		}

		basic_string& operator=(const basic_string& str)
		{
			return assign( str );
		}
		basic_string& operator=(const char_type* s)
		{
			return assign( s );
		}
		basic_string& operator=(char_type c)
		{
			return assign( 1, c );
		}
		// 21.3.2 iterators:
		iterator begin()
		{
			return my_str();
		}

		const_iterator begin() const
		{
			return my_str();
		}

		iterator end()
		{
			return begin() + size_;
		}
		
		const_iterator end() const
		{
			return begin() + size_;
		}

		reverse_iterator rbegin()
		{
			return reverse_iterator( end() );
		}
		const_reverse_iterator rbegin() const
		{			
			return  const_reverse_iterator( end() );
		}
		reverse_iterator rend()
		{
			return reverse_iterator( begin() );
		}
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator( begin() );
		}
		// 21.3.3 capacity:
		size_type size() const
		{
			return size_;
		}

		size_type length() const
		{
			return size_;
		}
		
		using base_type::max_size;

		void resize( size_type n, char_type c )
		{
			if( n > size_ )
			{
				//TODO: grow
				reserve( n );
				char_traits_type::assign( my_str() + size_, n - size_, c );
			}
			set_end( n );

		}

		void resize(size_type n)
		{
			resize( n, char_type() );
		}

		size_type capacity() const
		{
			return reserved_;
		}

		using base_type::reserve;
		

		void clear()
		{
			erase( begin(), end() );
		}

		bool empty() const
		{
			return begin() == end();
		}

		// 21.3.4 element access:
		const_reference operator[](size_type pos) const
		{
			return my_str()[ pos ];
		}

		reference operator[](size_type pos)
		{
			return my_str()[ pos ];
		}
		const_reference at(size_type n) const
		{
			if( n >= size() )
			{
				throw_out_of_range()
			}
			return my_str()[n];
		}

		reference at(size_type n)
		{
			if( n >= size() )
			{
				throw_out_of_range();
			}
			return my_str()[n];
		}

		// 21.3.5 modifiers:
		basic_string& operator+=(const basic_string& str)
		{
			return append( str );
		}

		basic_string& operator+=(const char_type* s)
		{
			return append( s );
		}

		basic_string& operator+=(char_type c)
		{
			return append( 1, c );
		}

		basic_string& append(const basic_string& str)
		{
			return append( str, 0, npos );
		}

		basic_string& append(const basic_string& str, size_type pos,
			size_type n)
		{
			size_type rlen = validate_pos_and_off( pos, str, n );

			insert( end(), str.begin() + pos, str.begin() + pos + rlen );
			return *this;
		}

		basic_string& append(const char_type* s, size_type n)
		{
			insert( end(), s, s + n );
			return *this;
		}

		basic_string& append(const char_type* s)
		{
			insert( end(), s, s + char_traits_type::length( s ) );
			return *this;
		}

		basic_string& append(size_type n, char_type c)
		{
			insert( end(), n, c );
			return *this;
		}

		template<class InputIterator>
		basic_string& append(InputIterator first, InputIterator last)
		{
			insert( end(), first, last );
			return *this;
		}


		void push_back(char_type c)
		{
			insert( end(), c );
		}
		
		//////////////////////////////////////////////////////////////////////////
		basic_string& assign(const basic_string& str)
		{
			return assign( str, 0, npos );
		}

		basic_string& assign(const basic_string& str, size_type pos,
			size_type n)
		{
			size_type rlen = validate_pos_and_off( pos, str, n );
			return assign( str.begin() + pos, str.begin() + pos + rlen );
		}

		basic_string& assign(const char_type* s, size_type n)
		{
			GSTL_ASSERT( s != 0 );
			GSTL_ASSERT( n < npos );

			set_end( 0 );
			insert( 0, s, n );
			return *this;
		}
		basic_string& assign(const char_type* s)
		{
			set_end( 0 );
			insert( 0, s );
			return *this;
		}
		basic_string& assign(size_type n, char_type c)
		{
			set_end( 0 );
			insert( begin(), n, c );
			return *this;
		}

		template<class InputIterator>
		basic_string& assign(InputIterator first, InputIterator last)
		{
			set_end( 0 );
			insert( begin(), first, last );
			return *this;
		}

		//////////////////////////////////////////////////////////////////////////
		basic_string& insert(size_type pos1, const basic_string& str)
		{
			return insert( pos1, str, 0, npos );
		}
		basic_string& insert(size_type pos1, const basic_string& str,
			size_type pos2, size_type n)
		{
			/*
			Requires pos1 <= size() and pos2 <= str.size()
			3 Throws: out_of_range if pos1 > size() or pos2 > str.size().
			4 Effects: Determines the effective length rlen of the string to insert as the smaller of n and str.size()
			- pos2. Then throws length_error if size() >= npos - rlen.
			Otherwise, the function replaces the string controlled by *this with a string of length size() +
			rlen whose first pos1 elements are a copy of the initial elements of the original string controlled by
			*this, whose next rlen elements are a copy of the elements of the string controlled by str beginning
			at position pos2, and whose remaining elements are a copy of the remaining elements of the original
			string controlled by *this.
			5 Returns: *this.
			*/
			
			verify_pos( pos1, *this );
			size_type rlen = validate_pos_and_off( pos2, str, n );
			if( size() > npos - rlen )
			{
				throw_length_error();
			}
			insert( begin() + pos1, str.begin() + pos2, str.begin() + pos2 + rlen );
			//grow( size() + rlen );
			//char_traits_type::copy( my_str() + pos1 + rlen, my_str() + pos1, rlen );
			//char_traits_type::copy( my_str() + pos1, str.c_str() + pos2, rlen );

			return *this;
		}
		basic_string& insert(size_type pos, const char_type* s, size_type n)
		{			
			verify_pos( pos, *this );
			insert( begin() + pos, s, s + n );
			return *this;
		}
		basic_string& insert(size_type pos, const char_type* s)
		{
			return insert( pos, s, char_traits_type::length( s ) );
		}

		basic_string& insert(size_type pos, size_type n, char_type c)
		{			
			verify_pos( pos, *this );
			insert( begin() + pos, n, c );
			return *this;
		}

		iterator insert(iterator p, char_type c)
		{			
			return insert( p, 1, c );
		}

		//Differs in return value from the C++ Standard
		iterator insert(iterator p, size_type n, char_type c)
		{//Most generic
			return do_replace( p, p, n, boost::bind( char_traits_type::assign, _1, n, c ) );
			
		}

		template<class InputIterator>
		void insert(iterator p, InputIterator first, InputIterator last)
		{//Most generic
			replace( p, p, first, last );			
		}

		//////////////////////////////////////////////////////////////////////////
		basic_string& erase( size_type pos = 0, size_type n = npos )
		{
			if( pos > size() )
			{
				throw_out_of_range();
			}
			iterator first = begin() + pos;
			iterator last = first + min( n, size() - pos );
			
			erase( first, last );
			return *this;
		}
		iterator erase( iterator position )
		{
			return erase( position, position + 1 );
		}
		iterator erase( iterator first, iterator last )
		{//Most generic
			GSTL_DEBUG_RANGE( first, last )
			if( first != last )
			{
				//TODO: erase chars
				//size_type rlen = last - first;
				char_traits_type::copy( &*first, &*last, end() - last );
				set_end( size() - (last - first) );
			}
			return last;
		}

		//////////////////////////////////////////////////////////////////////////
		basic_string& replace(size_type pos1, size_type n1,
			const basic_string& str)
		{
			return replace(pos1, n1, str, 0, npos);
		}

		basic_string& replace(size_type pos1, size_type n1,
			const basic_string& str,
			size_type pos2, size_type n2)
		{
			size_type xlen = validate_pos_and_off( pos1, *this, n1 );
			size_type rlen = validate_pos_and_off( pos2, str, n2 );
			//Throws: length_error if size() - xlen >= npos - rlen
			if( size() - xlen >= npos - rlen )
			{
				throw_length_error();
			}
			return replace( begin() + pos1, begin() + pos1 + xlen,
				str.begin() + pos2, str.begin() + pos2 + rlen);
		}

		basic_string& replace(size_type pos, size_type n1, const char_type* s,
			size_type n2)
		{
			size_type xlen = validate_pos_and_off( pos, *this, n1 );

			return replace( begin() + pos, begin() + pos + xlen,
				s, s + n2 );
		}
		basic_string& replace(size_type pos, size_type n1, const char_type* s)
		{
			return replace( pos, n1, s, char_traits_type::length( s ) );
		}

		basic_string& replace(size_type pos, size_type n1, size_type n2,
			char_type c)
		{
			size_type xlen = validate_pos_and_off( pos, *this, n1 );

			return replace( begin() + pos, begin() + pos + xlen,
				n, c );
		}

		basic_string& replace(iterator i1, iterator i2,
			const basic_string& str)
		{
			return replace( i1, i2,	str.begin(), str.end() );
		}

		basic_string& replace(iterator i1, iterator i2, const char_type* s,
			size_type n)
		{
			return replace( i1, i2,	s, s + n );
		}

		basic_string& replace(iterator i1, iterator i2, const char_type* s)
		{
			return replace( i1, i2,	s, char_traits_type::length( s ) );
		}

		basic_string& replace( iterator first, iterator last,
			size_type n, char_type c)
		{//Most generic
			do_replace( first, last, n, boost::bind( char_traits_type::assign, _1, n, c ) );
			return *this;
		}
	


		template<class InputIterator>
		basic_string& replace( iterator first, iterator last,
			InputIterator first2, InputIterator last2 )
		{//Most generic
			GSTL_DEBUG_RANGE( j1, j2 );
			//Length of new str
			difference_type xlen = gstl::distance( first2, last2 );
			
			do_replace( first, last, xlen, boost::bind( gstl::copy<InputIterator, iterator>, first2, last2, _1 ),
				check_overlap( first2, last2 ) );
			return *this;
		}

		template<class PutNew>
		iterator do_replace( iterator i1, iterator i2, difference_type xlen, PutNew put_new_fn, bool overlaped = false )
		{
			GSTL_DEBUG_RANGE( i1, i2 );
			size_t pos1 = i1 - begin();
			size_t pos2 = i2 - begin();
			size_type removed_size = i2 - i1;
			size_type new_size = size() + xlen - removed_size;

			pointer new_str;
			if( new_size >= reserved_ || overlaped )
			{//New string will be larger than current
				new_str = alloc_.allocate( new_size + 1 );
				//Copy to the new string [begin, i1) chars 
				char_traits_type::move( alloc_.address( new_str ), my_str(), pos1 );
			}
			else
			{//Existing string is long enough to store new one
				new_str = str_;
			}
			iterator new_i1 = iterator( alloc_.address( new_str + pos1 ) );
			//iterator new_i2 = iterator( alloc_.address( new_str + pos2 ) );
			iterator res = new_i1 + xlen;
			char_traits_type::move( &*res, my_str() + pos2, size() - pos2 );

			put_new_fn( new_i1 );

			reset_ptr( new_str, new_size, new_size + 1 );
			return res;
		}

		size_type copy(char_type* s, size_type n, size_type pos = 0) const
		{
			GSTL_ASSERT( s != 0 );
			
			size_type rlen = validate_pos_and_off( pos, *this, n );
			char_traits_type::move( s, my_str(), rlen );
			return rlen;
		}

		void swap(basic_string& str)
		{
			size_type tmp_size		= str.size_;
			size_type tmp_reserved	= str.reserved_;
			pointer tmp_str	= str.str_;

			str.size_		= size_;
			str.reserved_	= reserved_;
			str.str_		= str_;

			size_		= tmp_size;
			reserved_	= tmp_reserved;
			str_		= tmp_str;

		}

		// 21.3.6 string operations:
		const char_type* c_str() const // explicit
		{
			return my_str();
		}

		const char_type* data() const
		{
			return my_str();
		}

		allocator_type get_allocator() const
		{
			return alloc_;
		}
		
		size_type find (const basic_string& str, size_type pos = 0) const
		{
			return find( str.c_str(), pos, str.size() );
		}


		size_type find (const char_type* s, size_type pos = 0) const
		{
			return find( s, pos, char_traits_type::length( s ) );
		}

		size_type find (const char_type* s, size_type pos, size_type n) const
		{//Most generic
			if( n == 0 )
			{//Null string always matches, if it is inside the string
				return pos < size_ ? pos : npos;
			}
 
 			size_type sub_size = size_ - pos;
 			if( pos < size_ && n <= sub_size )
 			{	
				//Search for sub string
 				const char_type* fres_str;
				const char_type* my_sub_str = my_str() + pos;
				//Truncate search size 
				sub_size -= n - 1;
 				while( ( fres_str = char_traits_type::find( my_sub_str, sub_size, *s ) ) != 0 )
				{
					if( char_traits_type::compare(fres_str, s, n) == 0 )
					{
						//Match found
 						return fres_str - my_str();	
					}
					sub_size -= fres_str - my_sub_str + 1;
					my_sub_str = fres_str + 1;
				}
 			}

			//No match
			return npos;	
		}

		size_type find (char_type c, size_type pos = 0) const
		{			
			return find( &c, pos, 1);
		}

		size_type rfind(const basic_string& str, size_type pos = npos) const
		{
			return rfind( str.c_str(), pos, str.size() );
		}

		size_type rfind(const char_type* s, size_type pos, size_type n) const
		{//Most generic
 			if( n == 0 )
			{//Null string always matches, if it is inside the string
				return pos < size_ ? pos : npos;
			}

 			if( n <= size_ )
 			{	
				//Search for sub string in the reverse direction
				const char_type* fres_str = my_str() + gstl::min( pos, size_ - n );
				for( ;; --fres_str )
				{
 					if( char_traits_type::eq( *fres_str, *s )
 						&& char_traits_type::compare( fres_str, s, n ) == 0 )
					{
						//Match found
						return fres_str - my_str();
					}
					if( fres_str == my_str() )
					{
						//Beginning of the string reached
						break;
					}
				}
 			}

			//No match
			return npos;	
		}

		size_type rfind(const char_type* s, size_type pos = npos) const
		{
			return rfind( s, pos, char_traits_type::length( s ) );
		}

		size_type rfind(char_type c, size_type pos = npos) const
		{
			return rfind( &c, pos, 1);
		}

		size_type find_first_of(const basic_string& str,
			size_type pos = 0) const
		{
			return find_first_of( str.c_str(), pos, str.size() );
		}

		size_type find_first_of(const char_type* s,
			size_type pos, size_type n) const
		{
 			if( n > 0 && pos < size_ )
 			{	
				//Search for the first occurrence of any character from s in *this
 				const char_type *const my_end = my_str() + size_;
 				for( const char_type *curr_pos = my_str() + pos; curr_pos < my_end; ++curr_pos )
				{
					if( char_traits_type::find( s, n, *curr_pos ) != 0 )
					{
						//Match found
						return curr_pos - my_str();
					}
				}
 			}

			//No match
			return npos;
		}

		size_type find_first_of(const char_type* s, size_type pos = 0) const
		{
			return find_first_of(  s, pos, char_traits_type::length( s ) );
		}

		size_type find_first_of(char_type c, size_type pos = 0) const
		{
			return find_first_of( &c, pos, 1);
		}

		size_type find_last_of (const basic_string& str,
			size_type pos = npos) const
		{
			return find_last_of( str.c_str(), pos, str.size() );
		}

		size_type find_last_of (const char_type* s,
			size_type pos, size_type n) const
		{
			if( n > 0 && size_ )
			{
				//Search for the last occurrence of any character from s in *this
				const char_type *curr_pos = my_str() + gstl::min( pos, size_ - 1 );
				for( ;; --curr_pos )
				{
					if( char_traits_type::find( s, n, *curr_pos ) != 0 )
					{
						//Match found
						return curr_pos - my_str();
					}
 					if( curr_pos == my_str() )
					{
						//Beginning of the string reached
						break;
					}
				}
			}

			//no match
			return npos;
		}

		size_type find_last_of (const char_type* s, size_type pos = npos) const
		{
			return find_last_of( s, pos, char_traits_type::length( s ) );
		}

		size_type find_last_of (char_type c, size_type pos = npos) const
		{
			return find_last_of( &c, pos, 1);
		}

		size_type find_first_not_of(const basic_string& str,
			size_type pos = 0) const
		{
			return find_first_not_of( str.c_str(), pos, str.size() );
		}

		size_type find_first_not_of(const char_type* s, size_type pos,
			size_type n) const
		{
			if( n > 0 && pos < size_ )
			{
				//Search for the first occurrence of any character 
				//in *this not from s
 				const char_type *const my_end = my_str() + size_;
 				for( const char_type *curr_pos = my_str() + pos; curr_pos < my_end; ++curr_pos )
				{
					if( char_traits_type::find(s, n, *curr_pos) == 0 )
					{
						//Match found
						return curr_pos - my_str();
					}
				}
 			}

			//No match
			return npos;
		}

		size_type find_first_not_of(const char_type* s, size_type pos = 0) const
		{
			return find_first_not_of( s, pos, char_traits_type::length( s ) );
		}

		size_type find_first_not_of(char_type c, size_type pos = 0) const
		{
			return find_first_not_of( &c, pos, 1);
		}

		size_type find_last_not_of (const basic_string& str,
			size_type pos = npos) const
		{
			return find_last_not_of( str.c_str(), pos, str.size() );
		}

		size_type find_last_not_of (const char_type* s, size_type pos,
			size_type n) const
		{
			if( n > 0 && size_ )
			{
				//Search for the last occurrence of any character 
				//in *this not from s
				const char_type *curr_pos = my_str() + gstl::min( pos, size_ - 1 );
 				for( ;; --curr_pos )
				{
					if( char_traits_type::find( s, n, *curr_pos ) == 0 )
					{
						//Match found
						return curr_pos - my_str();
					}
 					if( curr_pos == my_str() )
					{
						//Beginning of the string reached
						break;
					}
				}
			}

			//No match
			return npos;
		}

		size_type find_last_not_of (const char_type* s,
			size_type pos = npos) const
		{
			return find_last_not_of( s, pos, char_traits_type::length( s ) );
		}


		size_type find_last_not_of (char_type c, size_type pos = npos) const
		{
			return find_last_not_of( &c, pos, 1);
		}

		basic_string substr(size_type pos = 0, size_type n = npos) const
		{
			return basic_string( *this, pos, n, alloc_ ) ;
		}

		int compare(const basic_string& str) const
		{
			return compare( 0, size(), str, 0, str.size() );
		}

		int compare(size_type pos1, size_type n1,
			const basic_string& str) const
		{
			return compare( pos1, n1, str, 0, str.size() );
		}

		int compare(size_type pos1, size_type n1,
			const basic_string& str,
			size_type pos2, size_type n2) const
		{
			verify_pos( pos2, str );
			return compare( pos1, n1, str.c_str() + pos2, n2 );
		}

		int compare(const char_type* s) const
		{
			return compare( 0, size(), s );
		}

		int compare(size_type pos1, size_type n1,
			const char_type* s) const
		{
			return compare( pos1, n1, s, char_traits_type::length( s ) );
		}

		int compare(size_type pos1, size_type n1,
			const char_type* s, size_type n2) const
		{
			//Most generic
			size_type len1 = validate_pos_and_off( pos1, *this, n1 );
			size_type rlen = gstl::min( len1, n2 );
			int cmp_res = char_traits_type::compare(data() + pos1, s, rlen);
			if( !cmp_res )
			{
				//size() < str.size() | < 0
				//size() == str.size()| 0
				//size() > str.size() | > 0
				return len1 - n2;
			}
			return cmp_res;
		}

	private:
		void verify_pos( size_type pos, const basic_string& str ) const
		{
			if( pos > str.size() )
			{
				throw_out_of_range();
			}
		}
		size_type validate_pos_and_off( size_type pos, const basic_string& str, size_type n ) const
		{
			verify_pos( pos, str );
			return min( n, str.size() - pos );
		}

		//TODO: split by categories types when implemented
		template<class InputIterator>
		bool check_overlap( InputIterator first, InputIterator last )
		{
			return false;
		}

		bool check_overlap( iterator first, iterator last )
		{
			return char_traits_type::check_overlap( &*begin(), &*end(), &*first, &*last ) != 0;
		}

		bool check_overlap( const char_type* first, const char_type* last )
		{
			return char_traits_type::check_overlap( &*begin(), &*end(), first, last ) != 0;
		}
	};


	template<class char_type, class traits, class allocator_type, class pointer_traits>
	basic_string<char_type, traits, allocator_type, pointer_traits>
		operator+(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return basic_string<char_type,traits,allocator_type, pointer_traits>( lhs ) += rhs;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	basic_string<char_type,traits,allocator_type, pointer_traits>
		operator+(const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return basic_string<char_type,traits,allocator_type, pointer_traits>( lhs ) += rhs;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	basic_string<char_type,traits,allocator_type, pointer_traits>
		operator+(char_type lhs, const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return basic_string<char_type,traits,allocator_type, pointer_traits>( lhs ) += rhs;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	basic_string<char_type,traits,allocator_type, pointer_traits>
		operator+(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return basic_string<char_type,traits,allocator_type, pointer_traits>( lhs ) += rhs;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	basic_string<char_type,traits,allocator_type, pointer_traits>
		operator+(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs, char_type rhs)
	{
		return basic_string<char_type,traits,allocator_type, pointer_traits>( lhs ) += rhs;
	}
	
	template<class char_type, class traits, class allocator_type, class pointer_traits >
	bool operator==(const basic_string<char_type, traits, allocator_type, pointer_traits>& lhs,
		const basic_string<char_type, traits, allocator_type, pointer_traits>& rhs)
	{
		return lhs.compare( rhs ) == 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator==(const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return rhs.compare( lhs ) == 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator==(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return lhs.compare( rhs ) == 0;
	}
	
	//////////////////////////////////////////////////////////////////////////
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator!=(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return rel_ops::operator !=( lhs, rhs );
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator!=(const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return rel_ops::operator !=( lhs, rhs );
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator!=(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return rel_ops::operator !=( lhs, rhs );
	}

	//////////////////////////////////////////////////////////////////////////
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator< (const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return lhs.compare( rhs ) < 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator< (const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return lhs.compare( rhs ) < 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator< (const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{//Params are in back order
		return rhs.compare( lhs ) > 0;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator> (const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return lhs.compare( rhs ) > 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator> (const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return lhs.compare( rhs ) > 0;
	}


	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator> (const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{//Params are in back order
		return rhs.compare( lhs ) < 0;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator<=(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return lhs.compare( rhs ) <= 0;
	}


	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator<=(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return lhs.compare( rhs ) <= 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator<=(const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{//Params are in back order
		return rhs.compare( lhs ) >= 0;
	}

	//////////////////////////////////////////////////////////////////////////
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator>=(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		return lhs.compare( rhs ) >= 0;
	}


	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator>=(const basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		const char_type* rhs)
	{
		return lhs.compare( rhs ) >= 0;
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	bool operator>=(const char_type* lhs,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{//Params are in back order
		return rhs.compare( lhs ) <= 0;
	}

	

	//////////////////////////////////////////////////////////////////////////
	// 21.3.7.8:
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	void swap(basic_string<char_type,traits,allocator_type, pointer_traits>& lhs,
		basic_string<char_type,traits,allocator_type, pointer_traits>& rhs)
	{
		lhs.swap( rhs );
	}

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	std::basic_istream<char_type,traits>&
		operator>>( std::basic_istream<char_type,traits>& is,
		basic_string<char_type,traits,allocator_type, pointer_traits>& str );
	

	template<class char_type, class traits, class stream_traits, class allocator_type, class pointer_traits>
	std::basic_ostream<char_type, stream_traits>&
		operator<<( std::basic_ostream<char_type, stream_traits>& os,
		const basic_string<char_type,traits,allocator_type, pointer_traits>& str )
	{
		return os << str.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	template<class char_type, class traits, class allocator_type, class pointer_traits>
	std::basic_istream<char_type,traits>&
		getline( std::basic_istream<char_type,traits>& is,
		basic_string<char_type,traits,allocator_type, pointer_traits>& str,
		char_type delim );

	template<class char_type, class traits, class allocator_type, class pointer_traits>
	std::basic_istream<char_type,traits>&
		getline( std::basic_istream<char_type,traits>& is,
		basic_string<char_type,traits,allocator_type, pointer_traits>& str );
	

	typedef basic_string<char, char_traits<char>, allocator<char> > string;
	typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > wstring;
}
// namespace std
// {
// 
// 	template<class char_type, class traits, class stream_traits, class allocator_type, class pointer_traits>
// 	std::basic_ostream<char_type, stream_traits>&
// 		operator<<( std::basic_ostream<char_type, stream_traits>& os,
// 		const gstl::basic_string<char_type,traits,allocator_type, pointer_traits>& str )
// 	{
// 		return os << str.c_str();
// 	}
// }

#endif //GSTL_BASIC_STRING_HEADER
