.TH EZMY 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
EZMY - 
Draws, in a manner determined by the current values of the
control parameters, a complete graph of one or more curves,
each defined by a set of points (I,YDRA(I,J)) (or
(I,YDRA(J,I)), depending on the current value of 'ROW.'),
for I from 1 to NPTS. The curve number J runs from 1 to
MANY. The argument GLAB may be used to specify a "graph
label", to be placed at the top of the graph.
.SH SYNOPSIS
CALL EZMY (YDRA,IDXY,MANY,NPTS,GLAB)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_ezmy (float *ydra, int idxy, int many, int npts, \\
.br
char *glab)
.SH DESCRIPTION
.IP YDRA 12
(an input array of type REAL, dimensioned IDXY x MANY
or IDXY x NPTS, depending on the current value of 'ROW.')
defines curve-point Y coordinates. The current value of
\&'NULL/1.' (default value "1.E36") may be used in YDRA to
signal missing points; curve segments on either side of a
missing point are omitted.
.RS
.IP \(bu
If 'ROW.' is positive (the default), the first subscript of
YDRA is a point number and the second is a curve number.
.IP \(bu
If 'ROW.' is negative, the order of the subscripts is
reversed ("row-wise", as opposed to "column-wise",
storage); the first subscript is a curve number and the
second is a point number.
.RE
.IP IDXY 12
(an input expression of type INTEGER) defines the
first dimension of the array YDRA, which is required by
EZMY in order to index the array properly.
.RS
.IP \(bu
If 'ROW.' is positive (the default), IDXY must be greater
than or equal to NPTS.
.IP \(bu
If 'ROW.' is negative, IDXY must be greater than or equal
to MANY.
.RE
.IP MANY 12
(an input expression of type INTEGER) is the number of
curves to be drawn by EZMY.
.IP NPTS 12
(an input expression of type INTEGER) is the number of
points defining each curve to be drawn by EZMY.
.IP GLAB 12
(an input expression of type CHARACTER) defines a new
"graph label". (If the first character of this expression
is "CHAR(0)", no new "graph label" is defined; the current
one will continue to be used.)  A character string defining
a new graph label must either be of the exact length
specified by the current value of 'LINE/MAXIMUM.' (default:
40 characters), or shorter; if shorter, it must be
terminated by the character defined by the current value of
\&'LINE/END.' (default: a '$'). The string becomes the new
text of line number 100 of the label named 'T'.
.SH C-BINDING DESCRIPTION
The C-binding argument description is the same as the FORTRAN 
argument description.
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
agex03,
tagupw,
tautog,
fagezmy.
.SH ACCESS 
To use EZMY or c_ezmy, load the NCAR Graphics libraries ncarg, ncarg_gks, 
and ncarg_c, preferably in that order.    To get smoother curves, 
drawn using spline interpolation, also load libdashsmth.o.  Or,
you can use the ncargf77 command to compile your program and load 
the above libraries, then, to get smoother curves, use the 
-dashsmth option.
.SH SEE ALSO
Online:
autograph,
agback,
agbnch,
agchax,
agchcu,
agchil,
agchnl,
agcurv,
agdshn,
aggetc,
aggetf,
aggeti,
aggetp,
aggetr,
agpwrt,
agrstr,
agsave,
agsetc,
agsetf,
agseti,
agsetp,
agsetr,
agstup,
agutol,
anotat,
displa,
ezmxy,
ezxy,
ezy
.sp
Hardcopy:
NCAR Graphics Fundamentals, UNIX Version
.SH COPYRIGHT
Copyright (C) 1987-2000
.br
University Corporation for Atmospheric Research
.br

This documentation is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This software is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
USA.
