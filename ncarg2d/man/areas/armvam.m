.TH ARMVAM 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
ARMVAM - Moves an area map from one integer array to another.
.SH SYNOPSIS
CALL ARMVAM (IAM,IAN,LAN)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_armvam (int *iam, int *ian, int lan)
.SH DESCRIPTION 
.IP "IAM" 12
(an input array of type INTEGER) - An array containing an area map that
has at least been initialized by a call to ARINAM.
.sp
Note: As part of initializing the area map, ARINAM stores the dimension of
MAP in MAP(1); therefore, the dimension does not have to be given as an
argument in calls to ARMVAM.)
.IP "IAN" 12
(an input array, dimensioned LAN, of type INTEGER) - An array to which the
area map in the array IAM is to be moved.
.IP "LAN" 12
(an input expression of type INTEGER) - The length of the array IAN.  This
may be less than or greater than the length of the array IAM.
.SH C-BINDING DESCRIPTION 
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH USAGE
This routine may be used for two purposes: 1) to pack an area map down into
the smallest possible space in the area map array; 2) during recovery from
an area map array overflow condition, to move the area map from a smaller
array to a larger array.
.sp
The area-map array IAM must at least have been initialized by a call to
ARINAM, and it may have appeared in calls to one or more of AREDAM, ARPRAM,
ARGTAI, ARDRLN, and ARSCAM. If the arrays IAM and IAN overlap in memory,
they must start at the same location in memory.
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
arex02.
.SH ACCESS
To use ARMVAM or c_armvam, load the NCAR Graphics libraries ncarg, ncarg_gks,
and ncarg_c, preferably in that order. 
.SH MESSAGES
See the areas man page for a description of all Areas error
messages and/or informational messages.
.SH SEE ALSO
Online:
areas, areas_params, ardbpa, ardrln, argeti, argetr, argtai, arinam,
arpram, arscam, arseti, arsetr, ncarg_cbind
.sp
Hardcopy:
NCAR Graphics Contouring and Mapping Tutorial
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
