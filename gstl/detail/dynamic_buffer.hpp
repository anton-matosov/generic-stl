/* 
Generic STL (genericstl)
http://genericstl.sourceforge.net/
Copyright (c) 2007, 2008 Anton (shikin) Matosov

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

#ifndef GSTL_DYNAMIC_SEQUENCE_HEADER
#define GSTL_DYNAMIC_SEQUENCE_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#	pragma once
#endif

#include <gstl/allocator>
#include <gstl/detail/buffer_helpers.hpp>
#include <gstl/detail/sequence_iterator.hpp>


namespace gstl
{
	namespace detail
	{
		template<class T>
		struct default_sequence_traits
		{
			static inline const T* move( T* dst, const T* src, size_t size )
			{	
				return detail::move( dst, src, size );
			}
		};

		template
		<
			class T,
			class Alloc,
			class Traits = default_sequence_traits<T>,
			class PtrTraits = typename Alloc::pointer_traits_type
		>
		class dynamic_buffer
		{
		public:
			enum{ min_buff_size = 4 };

			typedef dynamic_buffer							self_type;
			typedef typename Alloc::template rebind<T>::other	allocator_type;


			typedef typename allocator_type::value_type			value_type;
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::const_pointer		const_pointer;
			typedef typename allocator_type::reference			reference;
			typedef typename allocator_type::const_reference	const_reference;

			typedef typename allocator_type::size_type			size_type;
			typedef typename allocator_type::difference_type	difference_type;

			typedef Traits										traits_type;
			typedef PtrTraits									ptr_traits;

			pointer			buffer_;
			size_type		size_;
			size_type		reserved_;
			allocator_type	alloc_;


			dynamic_buffer( const allocator_type& alloc = allocator_type() )
				:buffer_( ptr_traits::null_ptr ),
				size_(0),
				reserved_(0),
				alloc_( alloc )
			{
			}
			
			~dynamic_buffer()
			{
				alloc_.deallocate( buffer_, reserved_ );
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
					pointer new_buffer = alloc_.allocate( n );
					
					if( !ptr_traits::is_null( buffer_ ) )
					{
						traits_type::move( alloc_.address( new_buffer ), get_buffer(), size_ );
					}
					reset_buffer( new_buffer, size_, n );
				}
			}

			void grow( size_type n )
			{
				reserve( n + 1 );
			}

			value_type* get_buffer()
			{
				return alloc_.address( buffer_ );
			}

			const value_type* get_buffer() const
			{
				return alloc_.address( buffer_ );
			}

			void throw_out_of_range() const
			{
				throw std::out_of_range( "invalid position" );
			}

			void throw_length_error() const
			{
				throw std::length_error( "invalid length" );
			}

			void reset_buffer( pointer new_buffer, size_type new_size, size_type new_reserved )
			{
				if( new_buffer != buffer_ )
				{
					pointer old_buffer = buffer_;
					buffer_ = new_buffer;
					alloc_.deallocate( old_buffer, reserved_ );
					reserved_ = new_reserved;
					set_size( new_size );
				}
			}

			void set_size( size_type new_size )
			{
				size_ = new_size;
			}

			size_type max_size() const
			{
				size_type max = alloc_.max_size();
				return max <= 1 ? 1 : max - 1;
			}

			void swap( self_type& rhs )
			{
				size_type tmp_size		= rhs.size_;
				size_type tmp_reserved	= rhs.reserved_;
				pointer tmp_buffer	= rhs.buffer_;

				rhs.size_		= size_;
				rhs.reserved_	= reserved_;
				rhs.buffer_		= buffer_;

				size_		= tmp_size;
				reserved_	= tmp_reserved;
				buffer_		= tmp_buffer;
			}

			size_type size() const
			{
				return size_;
			}

			size_type capacity() const
			{
				return reserved_;
			}

			bool empty() const
			{
				return !size();
			}

			// 21.3.4 element access:
			const_reference operator[](size_type pos) const
			{
				return get_buffer()[ pos ];
			}

			reference operator[](size_type pos)
			{
				return get_buffer()[ pos ];
			}

			const_reference at(size_type n) const
			{
				if( n >= size() )
				{
					throw_out_of_range();
				}
				return get_buffer()[n];
			}

			reference at(size_type n)
			{
				if( n >= size() )
				{
					throw_out_of_range();
				}
				return get_buffer()[n];
			}

			allocator_type get_allocator() const
			{
				return alloc_;
			}

		};
	}
}

#endif //GSTL_DYNAMIC_SEQUENCE_HEADER