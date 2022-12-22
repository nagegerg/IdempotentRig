# IdempotentRig

What are the elements of an idempotent rig with two generators?

See:  https://mathstodon.xyz/@johncarlosbaez/109544916566242548

 A 'rig' R has a commutative associative addition, an associative multiplication that distributes over addition, an element 0 with r+0 = r and 0r = 0 = r0 for all r ∈ R, and an element 1 with 1r = r = r1 for all r ∈ R.

 A rig is 'idempotent' if rr = r for all r ∈ R.
 
 With two generators a, b, along with the identity 1, we have 7 monomials:
 
 1, a, b, ab, ba, aba, bab
 
Because (1+1)^2 = (1+1), we have 4 = 2, 5 = 3, 6 = 2, 7 = 3, ....

So all elements can be written as 7-tuples of integers from 0 to 3, which are the
coefficients for the 7 monomials.

This program identifies all equivalence classes among the 4^7 7-tuples of integers,
and outputs them in algebraic form, first listing one representative of each class,
and then all the members of each class.

There turn out to be 284 equivalence classes, i.e. 284 distinct elements of the rig.
