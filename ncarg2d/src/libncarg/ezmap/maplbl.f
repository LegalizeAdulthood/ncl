C
C $Id: maplbl.f,v 1.16 2008-07-27 00:17:01 haley Exp $
C
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C The use of this Software is governed by a License Agreement.
C
      SUBROUTINE MAPLBL
        IF (ICFELL('MAPLBL - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
        CALL MDPLBL
        IF (ICFELL('MAPLBL',2).NE.0) RETURN
        RETURN
      END
