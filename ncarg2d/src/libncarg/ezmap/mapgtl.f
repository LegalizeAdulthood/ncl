C
C $Id: mapgtl.f,v 1.16 2008-07-27 00:17:00 haley Exp $
C
C                Copyright (C)  2000
C        University Corporation for Atmospheric Research
C                All Rights Reserved
C
C The use of this Software is governed by a License Agreement.
C
      SUBROUTINE MAPGTL (WHCH,LVAL)
        CHARACTER*(*) WHCH
        LOGICAL       LVAL
        IF (ICFELL('MAPGTL - UNCLEARED PRIOR ERROR',1).NE.0) RETURN
        CALL MDGETL (WHCH,LVAL)
        IF (ICFELL('MAPGTL',2).NE.0) RETURN
        RETURN
      END
