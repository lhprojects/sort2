#pragma once

#include <utility>
#include <algorithm>
// for std::swap
//     std::move

#include <iterator>
// for iterator_traits

#define _SORT_STD ::std::

#define _SORT_ ::sort::

namespace sort {

	int const _ISORT_MAX = 32;

	// TEMPLATE STRUCT less
	template<class _Ty = void>
	struct less
	{	// functor for operator<
		typedef _Ty first_argument_type;
		typedef _Ty second_argument_type;
		typedef bool result_type;

		constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{	// apply operator< to operands
			return (_Left < _Right);
		}
	};

	// TEMPLATE STRUCT SPECIALIZATION less
	template<>
	struct less<void>
	{	// transparent functor for operator<
		typedef int is_transparent;

		template<class _Ty1,
			class _Ty2>
			bool operator()(_Ty1 const &_Left, _Ty2 const &_Right) const
		{	// transparently apply operator< to operands
			return _Left < _Right;
		}
	};

	template<class _Ty1, class _Ty2>
	struct pair {
		pair(_Ty1 f, _Ty2 s) : first(f), second(s) {
		}

		_Ty1 first;
		_Ty2 second;
	};

	template<class _FwdIt1,
		class _FwdIt2> inline
		void iter_swap(_FwdIt1 _Left, _FwdIt2 _Right)
	{	// swap *_Left and *_Right
		std::swap(*_Left, *_Right);
	}

	template<class _FwdIt1,
		class _FwdIt2> inline
		void iter_copy(_FwdIt1 _Left, _FwdIt2 _Right)
	{	// copy *_Left to *_Right
		*_Left = *_Right;
	}

	template<class _FwdIt1,
		class _FwdIt2> inline
		void iter_move(_FwdIt1 _Left, _FwdIt2 _Right)
	{	// move *_Left to *_Right
		*_Left = std::move(*_Right);
	}


#define _SORT_LT_PRED(pred,x,y) pred(x, y)

	template<class _RanIt,
		class _Pr> inline
		void _Med3_unchecked(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr& _Pred)
	{	// sort median of three elements to middle
		if (_SORT_LT_PRED(_Pred, *_Mid, *_First))
			_SORT_ iter_swap(_Mid, _First);
		if (_SORT_LT_PRED(_Pred, *_Last, *_Mid))
		{	// swap middle and last, then test first again
			_SORT_ iter_swap(_Last, _Mid);
			if (_SORT_LT_PRED(_Pred, *_Mid, *_First))
				_SORT_ iter_swap(_Mid, _First);
		}
	}

	template<class _RanIt,
		class _Pr> inline
		void _Guess_median_unchecked(_RanIt _First, _RanIt _Mid, _RanIt _Last, _Pr& _Pred)
	{	// sort median element to middle
		if (40 < _Last - _First)
		{	// median of nine
			size_t _Step = (_Last - _First + 1) / 8;
			_SORT_ _Med3_unchecked(_First, _First + _Step, _First + 2 * _Step, _Pred);
			_SORT_ _Med3_unchecked(_Mid - _Step, _Mid, _Mid + _Step, _Pred);
			_SORT_ _Med3_unchecked(_Last - 2 * _Step, _Last - _Step, _Last, _Pred);
			_SORT_ _Med3_unchecked(_First + _Step, _Mid, _Last - _Step, _Pred);
		} else
			_SORT_ _Med3_unchecked(_First, _Mid, _Last, _Pred);
	}

	template<class _RanIt,
		class _Pr> inline
		pair<_RanIt, _RanIt>
		_Partition_by_median_guess_unchecked(_RanIt _First, _RanIt _Last, _Pr& _Pred)
	{	// partition [_First, _Last), using _Pred
		_RanIt _Mid = _First + (_Last - _First) / 2;
		_SORT_ _Guess_median_unchecked(_First, _Mid, _Last - 1, _Pred);
		_RanIt _Pfirst = _Mid;
		_RanIt _Plast = _Pfirst + 1;

		while (_First < _Pfirst
			&& !_SORT_LT_PRED(_Pred, *(_Pfirst - 1), *_Pfirst)
			&& !_Pred(*_Pfirst, *(_Pfirst - 1)))
			--_Pfirst;
		while (_Plast < _Last
			&& !_SORT_LT_PRED(_Pred, *_Plast, *_Pfirst)
			&& !_Pred(*_Pfirst, *_Plast))
			++_Plast;

		_RanIt _Gfirst = _Plast;
		_RanIt _Glast = _Pfirst;

		for (; ; )
		{	// partition
			for (; _Gfirst < _Last; ++_Gfirst)
				if (_SORT_LT_PRED(_Pred, *_Pfirst, *_Gfirst))
					;
				else if (_Pred(*_Gfirst, *_Pfirst))
					break;
				else if (_Plast++ != _Gfirst)
					_SORT_ iter_swap(_Plast - 1, _Gfirst);
			for (; _First < _Glast; --_Glast)
				if (_SORT_LT_PRED(_Pred, *(_Glast - 1), *_Pfirst))
					;
				else if (_Pred(*_Pfirst, *(_Glast - 1)))
					break;
				else if (--_Pfirst != _Glast - 1)
					_SORT_ iter_swap(_Pfirst, _Glast - 1);
			if (_Glast == _First && _Gfirst == _Last)
				return (pair<_RanIt, _RanIt>(_Pfirst, _Plast));

			if (_Glast == _First)
			{	// no room at bottom, rotate pivot upward
				if (_Plast != _Gfirst)
					_SORT_ iter_swap(_Pfirst, _Plast);
				++_Plast;
				_SORT_ iter_swap(_Pfirst++, _Gfirst++);
			} else if (_Gfirst == _Last)
			{	// no room at top, rotate pivot downward
				if (--_Glast != --_Pfirst)
					_SORT_ iter_swap(_Glast, _Pfirst);
				_SORT_ iter_swap(_Pfirst, --_Plast);
			} else
				_SORT_ iter_swap(_Gfirst++, --_Glast);
		}
	}

	// TEMPLATE FUNCTION make_heap WITH PRED
	template<class _RanIt,
		class _Pr> inline
		void _Make_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr& _Pred)
	{	// make nontrivial [_First, _Last) into a heap, using _Pred
		std::iterator_traits<_RanIt>::difference_type _Bottom = _Last - _First;
		if (2 <= _Bottom)
		{
			for (std::iterator_traits<_RanIt>::difference_type _Hole = _Bottom / 2; 0 < _Hole; )
			{	// reheap top half, bottom to top
				--_Hole;
				std::iterator_traits<_RanIt>::value_type _Val = _SORT_STD move(*(_First + _Hole));
				_SORT_ _Pop_heap_hole_by_index(_First, _Hole, _Bottom,
					_SORT_STD move(_Val), _Pred);
			}
		}
	}

	// TEMPLATE FUNCTION push_heap WITH PRED
	template<class _RanIt,
		class _Diff,
		class _Ty,
		class _Pr> inline
		void _Push_heap_by_index(_RanIt _First, _Diff _Hole,
			_Diff _Top, _Ty&& _Val, _Pr& _Pred)
	{	// percolate _Hole to _Top or where _Val belongs, using _Pred
		for (_Diff _Idx = (_Hole - 1) / 2;
			_Top < _Hole && _SORT_LT_PRED(_Pred, *(_First + _Idx), _Val);
			_Idx = (_Hole - 1) / 2)
		{	// move _Hole up to parent
			iter_move(_First + _Hole, _First + _Idx);
			_Hole = _Idx;
		}

		*(_First + _Hole) = _SORT_STD move(_Val);	// drop _Val into final hole
	}

	// TEMPLATE FUNCTION pop_heap WITH PRED
	template<class _RanIt,
		class _Diff,
		class _Ty,
		class _Pr> inline
		void _Pop_heap_hole_by_index(_RanIt _First, _Diff _Hole, _Diff _Bottom,
			_Ty&& _Val, _Pr& _Pred)
	{	// percolate _Hole to _Bottom, then push _Val, using _Pred
		// precondition: _Bottom != 0
		const _Diff _Top = _Hole;
		_Diff _Idx = _Hole;

		// Check whether _Idx can have a child before calculating that child's index, since
		// calculating the child's index can trigger integer overflows
		const _Diff _Max_sequence_non_leaf = (_Bottom - 1) / 2;
		while (_Idx < _Max_sequence_non_leaf)
		{	// move _Hole down to larger child
			_Idx = 2 * _Idx + 2;
			if (_SORT_LT_PRED(_Pred, *(_First + _Idx), *(_First + (_Idx - 1))))
				--_Idx;
			iter_move(_First + _Hole, _First + _Idx);
			_Hole = _Idx;
		}

		if (_Idx == _Max_sequence_non_leaf && _Bottom % 2 == 0)
		{	// only child at bottom, move _Hole down to it
			iter_move(_First + _Hole, _First + (_Bottom - 1));
			_Hole = _Bottom - 1;
		}

		_SORT_ _Push_heap_by_index(_First, _Hole, _Top, _SORT_STD move(_Val), _Pred);
	}

	template<class _RanIt,
		class _Ty,
		class _Pr> inline
		void _Pop_heap_hole_unchecked(_RanIt _First, _RanIt _Last, _RanIt _Dest,
			_Ty&& _Val, _Pr& _Pred)
	{	// pop *_First to *_Dest and reheap, using _Pred
		// precondition: _First != _Last
		// precondition: _First != _Dest
		iter_move(_Dest, _First);
		_SORT_ _Pop_heap_hole_by_index(_First, std::iterator_traits<_RanIt>::difference_type(0), std::iterator_traits<_RanIt>::difference_type(_Last - _First),
			_SORT_STD move(_Val), _Pred);
	}

	template<class _RanIt,
		class _Pr> inline
		void _Pop_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr& _Pred)
	{	// pop *_First to *(_Last - 1) and reheap, using _Pred
		if (2 <= _Last - _First)
		{
			--_Last;
			std::iterator_traits<_RanIt>::value_type _Val = _SORT_STD move(*_Last);
			_SORT_ _Pop_heap_hole_unchecked(_First, _Last, _Last,
				_SORT_STD move(_Val), _Pred);
		}
	}

	// TEMPLATE FUNCTION sort_heap WITH PRED
	template<class _RanIt,
		class _Pr> inline
		void _Sort_heap_unchecked(_RanIt _First, _RanIt _Last, _Pr& _Pred)
	{	// order heap by repeatedly popping, using _Pred
		for (; 2 <= _Last - _First; --_Last)
			_SORT_ _Pop_heap_unchecked(_First, _Last, _Pred);
	}


	// POINTER ITERATOR TAGS
	struct _General_ptr_iterator_tag
	{	// general case, no special optimizations
	};

	// TEMPLATE FUNCTION move_backward
	template<class _BidIt1,
		class _BidIt2> inline
		_BidIt2 _Move_backward_unchecked1(_BidIt1 _First, _BidIt1 _Last,
			_BidIt2 _Dest, _General_ptr_iterator_tag)
	{	// move [_First, _Last) backwards to [..., _Dest), no special optimization
		while (_First != _Last)
			iter_move(--_Dest, --_Last);
		return (_Dest);
	}

	template<class _BidIt1,
		class _BidIt2> inline
		_BidIt2 _Move_backward_unchecked(_BidIt1 _First, _BidIt1 _Last,
			_BidIt2 _Dest)
	{	// move [_First, _Last) backwards to [..., _Dest), choose optimization
		// note: _Move_backward_unchecked is called directly from elsewhere in the STL
		return (_Move_backward_unchecked1(_First, _Last,
			_Dest, _General_ptr_iterator_tag()));
	}

	// TEMPLATE FUNCTION sort WITH PRED
	template<class _BidIt,
		class _Pr> inline
		void _Insertion_sort_unchecked(_BidIt _First, _BidIt _Last, _Pr& _Pred)
	{	// insertion sort [_First, _Last), using _Pred
		if (_First != _Last)
			for (_BidIt _Next = _First; ++_Next != _Last; )
			{	// order next element
				_BidIt _Next1 = _Next;
				std::iterator_traits<_BidIt>::value_type _Val = _SORT_STD move(*_Next);

				if (_SORT_LT_PRED(_Pred, _Val, *_First))
				{	// found new earliest element, move to front
					_SORT_ _Move_backward_unchecked(_First, _Next, ++_Next1);
					*_First = _SORT_STD move(_Val);
				} else
				{	// look for insertion point after first
					for (_BidIt _First1 = _Next1;
						_SORT_LT_PRED(_Pred, _Val, *--_First1);
						_Next1 = _First1)
						iter_move(_Next1, _First1); 	// move hole down
					*_Next1 = _SORT_STD move(_Val);	// insert element in hole
				}
			}
	}

	template<class _RanIt,
		class _Diff,
		class _Pr> inline
		void _Sort_unchecked1(_RanIt _First, _RanIt _Last, _Diff _Ideal, _Pr& _Pred)
	{	// order [_First, _Last), using _Pred
		_Diff _Count;
		while (_ISORT_MAX < (_Count = _Last - _First) && 0 < _Ideal)
		{	// divide and conquer by quicksort
			pair<_RanIt, _RanIt> _Mid =
				_SORT_ _Partition_by_median_guess_unchecked(_First, _Last, _Pred);
			_Ideal /= 2, _Ideal += _Ideal / 2;	// allow 1.5 log2(N) divisions

			if (_Mid.first - _First < _Last - _Mid.second)
			{	// loop on second half
				_SORT_ _Sort_unchecked1(_First, _Mid.first, _Ideal, _Pred);
				_First = _Mid.second;
			} else
			{	// loop on first half
				_SORT_ _Sort_unchecked1(_Mid.second, _Last, _Ideal, _Pred);
				_Last = _Mid.first;
			}
		}

		if (_ISORT_MAX < _Count)
		{	// heap sort if too many divisions
			_SORT_ _Make_heap_unchecked(_First, _Last, _Pred);
			_SORT_ _Sort_heap_unchecked(_First, _Last, _Pred);
		} else if (2 <= _Count)
			_SORT_ _Insertion_sort_unchecked(_First, _Last, _Pred);	// small
	}

	template<class _RanIt,
		class _Pr> inline
		void _Sort_unchecked(_RanIt _First, _RanIt _Last, _Pr& _Pred)
	{	// order [_First, _Last), using _Pred
		_SORT_ _Sort_unchecked1(_First, _Last, _Last - _First, _Pred);
	}



	template<class _RanIt,
		class _Pr> inline
		void sort(_RanIt _First, _RanIt _Last, _Pr _Pred)
	{	// order [_First, _Last), using _Pred
		_SORT_ _Sort_unchecked(_First, _Last, _Pred);
	}

	// TEMPLATE FUNCTION sort
	template<class _RanIt> inline
		void sort(_RanIt _First, _RanIt _Last)
	{	// order [_First, _Last), using operator<
		_SORT_ sort(_First, _Last, less<>());
	}
}

