#ifndef __file_h_
#define __file_h_

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define GETPUTMETHODS(T) virtual T get##T() { T t; read(&t, sizeof(T)); return t; } \
                                                 virtual int32_t put##T(T t) { return write(&t, sizeof(T)); }

#define GETPUTFUNCS(T)	static inline T get##T(file &f) { return f.get##T(); }; \
                                                static inline int32_t put##T(file &f, T t) { return f.put##T(t); };

class  file
{
public:
  file()            { }
  virtual ~file()   { }

  virtual void      close()=0;

  virtual int32_t      read(void *buf, int32_t cnt)=0;
  virtual int32_t      write(const void *buf, int32_t cnt)=0;

  virtual int32_t      seek(int32_t pos=0)=0;
  virtual int32_t      seekcur(int32_t pos)   { return seek(tell()+pos); };
  virtual int32_t      seekend(int32_t pos=0) { return seek(size()+pos); };

  virtual int32_t      tell()=0;
  virtual int32_t      size()=0;

  virtual file&     operator [](int32_t pos) { seek(pos); return *this; }
  virtual bool     eof()                 { return tell()==size(); }

  virtual bool     eread(void *buf, int32_t cnt)  { return read(buf, cnt)==cnt; }
  virtual bool     ewrite(const void *buf, int32_t cnt) { return write(buf, cnt)==cnt; }

  virtual int32_t      copy(file &src, int32_t cnt=-1);

        template<class T> int32_t read(T& buf)   { return read(&buf,sizeof(T)); }
        template<class T> int32_t write(T& buf)  { return write(&buf,sizeof(T)); }

  GETPUTMETHODS(int)
  GETPUTMETHODS(bool)
  GETPUTMETHODS(int8_t)
  GETPUTMETHODS(int16_t)
  GETPUTMETHODS(int32_t)
  GETPUTMETHODS(int64_t)
  GETPUTMETHODS(uint8_t)
  GETPUTMETHODS(uint16_t)
  GETPUTMETHODS(uint32_t)
  GETPUTMETHODS(uint64_t)
  GETPUTMETHODS(float)
  GETPUTMETHODS(double)

        virtual int32_t puts(const char * t) { if (t) return write((void *)t,(int32_t)strlen(t)); else return 0; }
};


class fileS: public file
{
private:
        FILE* f = NULL;

public:
        fileS()   {}
  ~fileS()  { close(); }

	enum {
	  rd=0, wr=1, rw=2,
	  ex=0, cr=4, up=8
	};

        virtual bool     open(const char *name, int mode=rd|ex);
        virtual int32_t      read(void *buf, int32_t cnt);
        virtual int32_t      write(const void *buf, int32_t cnt);

        virtual void      close()			      { if (f != NULL) { fclose(f); f = NULL; } }

        virtual int32_t      seek(int32_t pos)	  { return fseek(f, pos, SEEK_SET); }
        virtual int32_t      seekcur(int32_t pos) { return fseek(f, pos, SEEK_CUR); }
        virtual int32_t      seekend(int32_t pos) { return fseek(f, pos, SEEK_END); }

        virtual int32_t      tell()			      { return ftell(f); }
        virtual int32_t      size()			      { long int pos = ftell(f); fseek(f, 0, SEEK_END); long int s = ftell(f); fseek(f, pos, SEEK_SET); return s; }

        template<class T> int32_t read(T& buf)   { return read(&buf,sizeof(T)); }
        template<class T> int32_t write(T& buf)  { return write(&buf,sizeof(T)); }
};

/*
class fileM: public file
{
protected:
        uint8_t *fbuf;
        int32_t  fpos, flen;
        bool dwc;
        bool readonly;

public:
	fileM()
	{
		fbuf=0;
		flen=0;
		fpos=0;
                dwc = false;
                readonly = false;
	}

  ~fileM()
  {
    close();
  }

        virtual bool open(void *buf, int32_t len, bool deleteMemOnClose = false)
	{
		close();

                fbuf = (uint8_t *)buf;
                flen = len;
                dwc = deleteMemOnClose;

		return (fbuf && flen) ? sTRUE : sFALSE;
	}

        virtual bool open(int resID)
	{
    HINSTANCE inst;
    HRSRC     res;
    HGLOBAL   rptr;

    inst=GetModuleHandle(0);
		if (!(res=FindResource(inst, MAKEINTRESOURCE(resID), RT_RCDATA)))
		  return sFALSE;

		if (!(rptr=LoadResource(inst, res)))
		  return sFALSE;

		readonly=sTRUE;
		
    return open(LockResource(rptr), SizeofResource(inst, res));
	}

        virtual bool open(file &f, int32_t len=-1)
	{
                int32_t maxlen=f.size()-f.tell();
		if (len>=0 && len<maxlen)
			maxlen=len;

		if (!openNew(maxlen))
			return sFALSE;

		flen=f.read(fbuf,flen);
		return sTRUE;
	}

        virtual bool openNew(int32_t len)
	{
		close();

    if (len)
    {
		  fbuf=new sU8[len];
		  if (fbuf) 
      {
			  flen=len;
			  dwc=sTRUE;
		  }
    }

		return (fbuf && flen) ? sTRUE : sFALSE;
	}

	virtual void *detach()
	{
		if (!dwc || !fbuf)
			return 0;

		void *r=fbuf;
		fbuf=0;
		flen=0;
		fpos=0;

		return r;
	}

	virtual void close()
	{
		if (fbuf && dwc)
			delete[] fbuf;
    
		fbuf=0;
		flen=0;
		fpos=0;
	}

        virtual int32_t read(void *buf, int32_t cnt)
	{
		if (!buf || !fbuf)
			return 0;

		if (cnt<0) 
      cnt=0;

		if (cnt>(flen-fpos))
      cnt=flen-fpos;

		memcpy(buf, fbuf+fpos, cnt);
		fpos+=cnt;

		return cnt;
	}

        virtual int32_t write(const void *buf, int32_t cnt)
	{
		if (!buf || !fbuf || readonly)
			return 0;

		if (cnt<0)
      cnt=0;

		if (cnt>(flen-fpos))
      cnt=flen-fpos;

		memcpy(fbuf+fpos, buf, cnt);
		fpos+=cnt;
		return cnt;
	}

        virtual int32_t seek(int32_t pos)
	{
		if (pos<0)
      pos=0;

		if (pos>flen)
      pos=flen;

		return fpos=pos;
	}

        virtual int32_t tell() { return fpos; }
        virtual int32_t size() { return flen; }

        template<class T> int32_t read(T& buf)   { return read(&buf,sizeof(T)); }
        template<class T> int32_t write(T& buf)  { return write(&buf,sizeof(T)); }
};
*/

class fileA: public file
{
protected:
	file  *fhost;
        int32_t  foffset, flen, fpos;
        bool dhc;

public:
	fileA()
	{
	  fhost=0;
	  foffset=0;
	  flen=0;
	  fpos=0;
	  dhc=0;
	}

  ~fileA()
  {
    close();
  }

        virtual bool open(file &f, int32_t pos, int32_t len, bool deleteHostOnClose = false)
	{
          int32_t fsize=f.size();
	  fhost=&f;

    pos=(pos<0)?0:(pos>fsize)?fsize:pos;
	  if (len<0)
      len=0;

	  if ((pos+len)>fsize)
      len=fsize-pos;

	  f.seek(pos);
	  foffset=pos;

	  flen=len;
	  dhc=deleteHostOnClose;
	  fpos=0;

          return (flen) ? true : false;
	}

  virtual void close()
	{
	  if (fhost && dhc)
	  {
		  fhost->close();
		  delete fhost;
	  }

	  fhost=0;
	  foffset=0;
	  flen=0;
	  fpos=0;
	}

        virtual int32_t read(void *buf, int32_t cnt)
	{
		if (!buf || !fhost)
			return 0;

		if (cnt<0)
      cnt=0;
		if (cnt>(flen-fpos))
      cnt=flen-fpos;

		cnt=fhost->read(buf,cnt);
		fpos+=cnt;

		return cnt;
	}

        virtual int32_t write(const void *buf, int32_t cnt)
	{
		if (!buf || !fhost)
			return 0;

		if (cnt<0)
      cnt=0;

		if (cnt>(flen-fpos))
      cnt=flen-fpos;

		cnt=fhost->write(buf,cnt);
		fpos+=cnt;

		return cnt;
	}

        virtual int32_t seek(int32_t pos)
	{
		if (pos<0)
      pos=0;

		if (pos>flen)
      pos=flen;

		fhost->seek(foffset+pos);

		return fpos=fhost->tell()-foffset;
	}

        virtual int32_t seekcur(int32_t pos)
	{
		fhost->seekcur(pos);
		return fpos=fhost->tell()-foffset;
	}

        virtual int32_t tell() { return fpos; }
        virtual int32_t size() { return flen; }

        template<class T> int32_t read(T& buf)   { return read(&buf,sizeof(T)); }
        template<class T> int32_t write(T& buf)  { return write(&buf,sizeof(T)); }
};

#define FILEMTMP_BLOCKSIZE 65536

class fileMTmp: public file
{
protected:
	struct tmblock 
  {
		tmblock()
    {
      next=0; 
    }

		~tmblock() 
		{ 
			if (next)
				delete next; 
		}

		tmblock *next;
                uint8_t content[FILEMTMP_BLOCKSIZE];
	};

	tmblock *firstblk, *curblk;
        int32_t    bpos, fpos, flen;

public:
	fileMTmp()
	{
		firstblk=0;
	  curblk=0;
		flen=0;
		fpos=0;
		bpos=0;
	}

  ~fileMTmp()
  {
    close();
  }

        virtual bool open()
	{
		close();
		firstblk=new tmblock;
		seek(0);

                return firstblk ? true : false;
	}

	virtual void close()
	{
		if (firstblk)
			delete firstblk;

		firstblk=0;
	  curblk=0;
		flen=0;
		fpos=0;
		bpos=0;
	}

        virtual int32_t read(void *buf, int32_t cnt)
	{
		if (!buf || !curblk)
			return 0;

		if (cnt>(flen-fpos))
      cnt=flen-fpos;

                uint8_t *out=(uint8_t *) buf;
                int32_t  rb=0;

		while (cnt)
		{
			// noch im selben block?
			if (bpos+cnt<FILEMTMP_BLOCKSIZE)
			{
				memcpy(out, curblk->content+bpos, cnt);
				out+=cnt;
				bpos+=cnt;
				rb+=cnt;
				cnt=0;
			}
      else
			{
				// rest vom block übertragen
                                int32_t tocopy=FILEMTMP_BLOCKSIZE-bpos;
				memcpy(out, curblk->content+bpos, tocopy);

				out+=tocopy;
				rb+=tocopy;
				cnt-=tocopy;
				bpos=0;
				curblk=curblk->next;

				if (!curblk)
					cnt=0;
			}
		}

		fpos+=rb;
		return rb;
	}

        virtual int32_t write(const void *buf, int32_t cnt)
	{
		if (!buf || !curblk)
			return 0;

		if (cnt<0) 
			cnt=0;

                uint8_t *in=(uint8_t *) buf;
                int32_t  wb=0;

		while (cnt)
		{
			// noch im selben block?
			if (bpos+cnt<FILEMTMP_BLOCKSIZE)
			{
				memcpy(curblk->content+bpos, in, cnt);
				in+=cnt;
				bpos+=cnt;
				wb+=cnt;
				cnt=0;
			}
      else
			{
				// rest vom block übertragen
                                int32_t tocopy=FILEMTMP_BLOCKSIZE-bpos;
				memcpy(curblk->content+bpos, in, tocopy);
				in+=tocopy;
				wb+=tocopy;
				cnt-=tocopy;
				bpos=0;
				// nexter block (oder nen neuer)
				if (!curblk->next)
					curblk->next=new tmblock;

				curblk=curblk->next;
				if (!curblk)
					cnt=0;
			}
		}

		fpos+=wb;
		if (fpos>flen) flen=fpos;
		return wb;
	}

        virtual int32_t seek(int32_t pos)
	{
    pos=(pos<0)?0:(pos>flen)?flen:pos;

                int32_t pos2=pos;
		curblk=firstblk;

		while (pos2>=FILEMTMP_BLOCKSIZE && curblk)
		{
			curblk=curblk->next;
			pos2-=FILEMTMP_BLOCKSIZE;
		}
		bpos=pos2;

		return fpos=pos;
	}

        virtual int32_t tell() { return fpos; }
        virtual int32_t size() { return flen; }

        template<class T> int32_t read(T& buf)   { return read(&buf, sizeof(T)); }
        template<class T> int32_t write(T& buf)  { return write(&buf, sizeof(T)); }

};

GETPUTFUNCS(bool)
GETPUTFUNCS(int)
GETPUTFUNCS(int8_t)
GETPUTFUNCS(int16_t)
GETPUTFUNCS(int32_t)
GETPUTFUNCS(int64_t)
GETPUTFUNCS(uint8_t)
GETPUTFUNCS(uint16_t)
GETPUTFUNCS(uint32_t)
GETPUTFUNCS(uint64_t)
GETPUTFUNCS(float)
GETPUTFUNCS(double)

#define bread(f,v)  (f).eread(&v,sizeof(v))
#define bwrite(f,v) (f).ewrite(&v,sizeof(v))


#endif
