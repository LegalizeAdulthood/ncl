C
C	$Id: wmfpts.f,v 1.1 1994-09-09 23:54:57 fred Exp $
C
      SUBROUTINE WMFPTS(M,X,Y,N,XT,YT)
C
C  Filter out N points from the coordinates (X(I),Y(I),I=1,M) and
C  store them in (XT(I),YT(I),I=1,N).  It is assumed that M > N.
C
      DIMENSION X(M),Y(M),XT(N),YT(N)
C
      SCL = REAL(N-1)/REAL(M-1)
      OFF = REAL(M-N)/REAL(M-1)
C
      IX = 1
      XT(1) = X(1)
      YT(1) = Y(1)
      DO 100 I=2,M
        STMP = SCL*REAL(I)+OFF
        IT = MIN(INT(STMP),N)
        IF (IT .EQ. IX) GO TO 100
        IX = IT
        XT(IX) = X(I)
        YT(IX) = Y(I)
  100 CONTINUE
      XT(N) = X(M)
      YT(N) = Y(M)
C
      RETURN
      END
