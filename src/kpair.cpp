#include <cassert>

#include "kpair.hpp"
#include "kernel.hpp"
#include "portorder.hpp"

kpair::kpair( raft::kernel &a, 
              raft::kernel &b,
              const bool split,
              const bool join ) : kpair( a, b )
{
    split_to = split;
    join_from = join;
}

/**
 * kpair - for joining kpair on the left (src)
 * and dst on right.
 */
kpair::kpair( kpair &a, 
              raft::kernel &b,
              const bool split,
              const bool join ) : kpair( *(a.dst), b )
{
    head        = a.head;
    a.next  = this;
    split_to    = split;
    join_from   = join;
}
/**
 * opposite of above 
 */
kpair::kpair( raft::kernel &a,
              kpair        &n, 
              const bool split,
              const bool join ) : kpair( a, *(n.src) )
{
    head        = this;
    next        = &n;
    split_to    = split;
    join_from   = join;
}

kpair::kpair( kpair &a,
              kpair &b, 
              const bool split,
              const bool join ) : kpair( *(a.dst), *(b.src) )
{
    head = a.head;
    a.next = this;
    b.head = a.head;
    next = &b;
    split_to  = split;
    join_from = join;
}

kpair::kpair( raft::kernel &a, raft::kernel &b )
{
    src = &a;
    src_name = a.getEnabledPort();
    if( src_name.length() > 0 )
    {
        /** set false by default **/
        has_src_name = true;
    }
    dst = &b;
    dst_name = b.getEnabledPort();
    if( dst_name.length() > 0 )
    {
        /** set false by default **/
        has_dst_name = true;
    }
    src_out_count = a.output.count();
    dst_in_count  = b.input.count();
    head = this;
}

void 
kpair::setOoO() noexcept
{
    (this)->out_of_order = true;
    return;
}

kpair& 
operator >> ( raft::kernel &a, raft::kernel &b )
{
    auto *ptr( new kpair( a, b ) );
    return( *ptr );
}

kpair&
operator >> ( raft::kernel &&a, raft::kernel &&b )
{
    auto *ptr( new kpair( a, b ) );
    return( *ptr );
}

kpair&  
operator >> ( kpair &a, raft::kernel &b )
{
    auto *ptr( new kpair( a, b, false, false ) );
    return( *ptr );
}

kpair&
operator >> ( kpair &a, raft::kernel &&b )
{
    auto *ptr( new kpair( a, b, false, false ) );
    return( *ptr );
}

/**
 * >>, we're using the raft::order::spec as a linquistic tool
 * at this point. It's only used for disambiguating functions.
 */
LOoOkpair&
operator >> ( raft::kernel &a, const raft::order::spec &&order )
{
    UNUSED( order );
    auto *ptr( new LOoOkpair( a ) );
    return( *ptr );
}

kpair&
operator >> ( LOoOkpair &a, raft::kernel &b )
{
    auto *ptr( 
        new kpair( a.kernel, 
                   b, 
                   false, 
                   false ) 
    );
    delete( &a );
    ptr->setOoO();
    return( *ptr );
}


kpair&
operator >> ( LOoOkpair &a, raft::kernel &&b )
{
    auto *ptr( 
        new kpair( a.kernel, b, false, false ) 
    );
    delete( &a );
    ptr->setOoO();
    return( *ptr );
}

/**
 * >>, we're using the raft::order::spec as a linquistic tool
 * at this point. It's only used for disambiguating functions.
 */
ROoOkpair& 
operator >> ( kpair &a, const raft::order::spec &&order )
{
    auto *ptr( new ROoOkpair( a ) );
    UNUSED( order );
    return( *ptr );
}

kpair&
operator >> ( ROoOkpair &a, raft::kernel &b )
{
    auto * ptr(
        new kpair( a.other, b, false, false )
    );
    delete( &a );
    ptr->setOoO();
    return( *ptr );
}

kpair&
operator >> ( ROoOkpair &a, raft::kernel &&b )
{
    auto * ptr(
        new kpair( a.other, b, false, false )
    );
    delete( &a );
    ptr->setOoO();
    return( *ptr );
}


kpair&
operator <= ( raft::kernel &a, raft::kernel &b )
{
    auto *ptr( new kpair( a, b, true, false ) );
    return( *ptr );
}

kpair&
operator <= ( raft::kernel &&a, raft::kernel &&b )
{
    auto *ptr( new kpair( a, b, true, false ) );
    return( *ptr );
}

kpair&
operator <= ( raft::kernel &a, kpair &b )
{
    auto *ptr( new kpair( a, b, true, false ) );
    return( *ptr );
}

kpair&
operator <= ( raft::kernel &&a, kpair &b )
{
    auto *ptr( new kpair( a, b, true, false ) );
    return( *ptr );
}

kpair&
operator >= ( kpair &a, raft::kernel &&b )
{
    auto *ptr( new kpair( a, b, false, true ) );
    return( *ptr );
}

kpair&
operator >= ( kpair &a, raft::kernel &b )
{
    auto *ptr( new kpair( a, b, false, true ) );
    return( *ptr );
}

kpair&
operator >= ( kpair &a, kpair &b )
{
    auto *ptr( new kpair( a, b, false, true ) );
    return(*ptr);
}

kpair& 
operator >= ( raft::kernel &a, kpair &b )
{
    auto *ptr( new kpair( a, b, false, true ) );
    return(*ptr);
}

kpair& 
operator >= ( raft::kernel &&a, kpair &b )
{
    auto *ptr( new kpair( a, b, false, true ) );
    return(*ptr);
}
