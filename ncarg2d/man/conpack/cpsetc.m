.TH CPSETC 3NCARG "March 1993" UNIX "NCAR GRAPHICS"
.na
.nh
.SH NAME
CPSETC - Sets the value of an internal parameter of
type CHARACTER.
.SH SYNOPSIS
CALL CPSETC (PNAM, CVAL)
.SH C-BINDING SYNOPSIS
#include <ncarg/ncargC.h>
.sp
void c_cpsetc (char *pnam, char *cval)
.SH DESCRIPTION 
.IP PNAM 12
(CHARACTER, input) is the name of a parameter to be 
given a character value. Only the first three characters of 
PNAM are examined. It is recommended that the rest of the 
character string be used to improve the readability of the 
code. For example, instead of 'ILT', use 'ILT - 
INFORMATIONAL LABEL TEXT'.
.IP CVAL 12
(CHARACTER, input) is a character constant or variable, the value of
which is to be given to the parameter specified by PNAM.
.SH C-BINDING DESCRIPTION
The C-binding argument descriptions are the same as the FORTRAN 
argument descriptions.
.SH USAGE
This routine allows you to set the current value of
Conpack parameters.  For a complete list of parameters available
in this utility, see the conpack_params man page.
.SH EXAMPLES
Use the ncargex command to see the following relevant
examples: 
cpex02,
cpex04,
cpex05,
cpex06,
cbex01.
.SH ACCESS
To use CPSETC, load the NCAR Graphics libraries ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order.  To use c_cpsetc, 
load the NCAR Graphics libraries ncargC, ncarg_gksC, ncarg, ncarg_gks,
ncarg_c, and ncarg_loc, preferably in that order.
.SH MESSAGES
See the conpack man page for a description of all Conpack error
messages and/or informational messages.
.SH SEE ALSO
Online:
conpack,
conpack_params,
cpback, cpchcf, cpchcl, cpchhl, cpchil, cpchll, cpcica, cpclam, cpcldm,
cpcldr, cpcltr, cpcnrc, cpdrpl, cpezct, cpgetc, cpgeti, cpgetr, cplbam,
cplbdr, cpmpxy, cppkcl, cppklb, cprect, cprset, cpscae, cpseti,
cpsetr, cpsps1, cpsps2, ncarg_cbind
.sp
Hardcopy:
Tutorial: A Step-by-Step Guide to Contouring and Mapping
.SH COPYRIGHT
Copyright 1987, 1988, 1989, 1991, 1993 University Corporation
for Atmospheric Research
.br
All Rights Reserved

