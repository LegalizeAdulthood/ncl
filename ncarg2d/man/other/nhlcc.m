.\"
.\"	$Id: nhlcc.m,v 1.9 2000-08-22 04:16:28 haley Exp $
.\"
.TH NHLCC 1NCARG "April 1995" NCAR "NCAR GRAPHICS"
.SH NAME
nhlcc \- Command for compiling C code that uses the NCAR Graphics
high-level utilities.
.SH SYNOPSIS
\fBnhlcc\fP 
[\fB\-ngmath\fR]
[\fB\-netcdf\fR]
[\fB\-hdf\fR] ...
.SH DESCRIPTION
\fInhlcc\fP is a script that invokes the C compiler/linker
with the proper NCAR Graphics LLU (low-level utility) and HLU
(high-level utility) libraries.  Arguments presented above are
associated with NCAR Graphics.  All other arguments and options are
identical to the cc command on your particular machine; arguments
that include quoted strings may have to be enclosed in single quotes.
.sp
If you don't want to use \fInhlcc\fP, you can just type it on the
command line to see what gets included in the link line, and then you
can add this information to your own Makefile or script.  It is
important to note that you must define the macro \fINeedFuncProto\fP
in order for function prototyping to work correctly.
.sp
In order to run \fInhlcc\fP, you must have your NCARG_ROOT
environment variable set to the directory pathname where the NCAR
Graphics libraries, binaries, and include files were installed.  If
you are not sure what NCARG_ROOT should be set to, please check with 
your system administrator or the site representative for NCAR Graphics.
If the NCAR Graphics libraries, binaries, and include files were not
installed under one root directory, then you will need to set the 
environment variables NCARG_LIB, NCARG_BIN, and NCARG_INCLUDE instead.
Please see "man ncargintro" for more information.
.sp
Note that, on some systems, if you supply your own binary libraries in
addition to the ones automatically referenced by \fInhlcc\fR, all the
libraries must have been created in a similar fashion.
.sp
.I OPTIONS
.IP "\-ngmath"
Links in the NCAR Graphics ngmath library.
.sp
.IP "\-netcdf"
Links in the netCDF library.  This library is not part of NCAR Graphics,
so check with your system administrator if you need it installed.  You
can obtain a copy of it by doing an anonymous ftp to unidata.ucar.edu.
.sp
.IP "\-hdf"
Links in the HDF library.  This library is not part of NCAR Graphics,
so check with your system administrator if you need it installed.  You
can obtain a copy of it by doing an anonymous ftp to ftp.ncsa.uiuc.edu.
.sp
.SH SEE ALSO
Online:
.BR nhlf77(1NCARG),
.BR ncargf77(1NCARG),
.BR ncargcc(1NCARG),
.BR ncargintro(5NCARG)
.sp
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
