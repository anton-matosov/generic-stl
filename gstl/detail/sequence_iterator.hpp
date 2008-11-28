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

#ifndef GSTL_SEQUENCE_ITERATOR_HEADER
#define GSTL_SEQUENCE_ITERATOR_HEADER

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#	pragma once
#endif

#include <boost/iterator/iterator_adaptor.hpp>
#include <gstl/detail/iterator_declarer.hpp>

namespace gstl
{
	namespace detail
	{
 		template <class PtrT, class ContainerT>
 		class sequence_iterator
 			: public boost::iterator_adaptor<
 			sequence_iterator<PtrT, ContainerT>		// Derived
 			, PtrT									// Base
 			, boost::use_default					// Value
 			, boost::random_access_traversal_tag	// CategoryOrTraversal
 			>
 		{
 		private:
 			struct enabler {};  // a private type avoids misuse

			typedef PtrT									pointer_type;
			typedef ContainerT								container_type;
 		public:
 			typedef sequence_iterator						self_type;
 			typedef typename self_type::iterator_adaptor_	base_type;
 			typedef typename base_type::value_type			value_type;
 
 			sequence_iterator()
 				: base_type( 0 )
 			{}
 
 			explicit sequence_iterator( pointer_type p )
 				: base_type( p )
 			{}
 
			template <class OtherPtrT>
			sequence_iterator( sequence_iterator<OtherPtrT, container_type> const& other,
				typename boost::enable_if< boost::is_convertible<OtherPtrT, pointer_type>,
				enabler >::type = enabler() )
				: base_type( other.base() )
			{

			}

			
 		};

		template <class ContainerT>
		class declare_sequence_iterator
			:public detail::iterator_declarer<ContainerT, sequence_iterator>
		{
		
		};
	}
}

#endif //GSTL_SEQUENCE_ITERATOR_HEADER
