#ifndef _CH_STATE_H_
#define _CH_STATE_H_
// Copyright(c) 2016 Yohei Matsumoto, All right reserved. 

// ch_state.h is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ch_state.h is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ch_state.h.  If not, see <http://www.gnu.org/licenses/>.

#include "../util/aws_coord.h"

#include "ch_base.h"

class ch_state: public ch_base
{
 protected:
	 long long tatt, tpos, tvel, tdp;
	 long long tattf, tposf, tvelf, tdpf;
  float roll, pitch, yaw; // roll(deg), pitch(deg), yaw(deg)
  float lon, lat, alt, galt; // longitude(deg), latitude(deg), altitude(m), geoid altitude(m)
  float x, y, z; // ecef coordinate
  Mat R, Rret; // Rotation matrix for ENU transformation
  float cog, sog; // Course over ground(deg), Speed over ground (kts)
  float depth; // water depth
  long long m_tfile;
  float rollf, pitchf, yawf; // roll(deg), pitch(deg), yaw(deg)
  float lonf, latf, altf, galtf; // longitude(deg), latitude(deg), altitude(m), geoid altitude(m)
  float xf, yf, zf; // ecef coordinate
  float cogf, sogf; // Course over ground(deg), Speed over ground (kts)
  float depthf; // water depth

 public:
 ch_state(const char * name): ch_base(name), 
	 m_tfile(0),
	 tatt(0), tpos(0), tvel(0), tdp(0), tattf(0), tposf(0), tvelf(0), tdpf(0), 
	 roll(0), pitch(0), yaw(0),
    lon(0), lat(0), alt(0), galt(0), x(0), y(0), z(0), cog(0), sog(0), depth(0)
    {
		R = Mat::eye(3, 3, CV_64FC1);
    }

  void set_attitude(const long long _tatt, const float _r, const float _p, const float _y)
  {
    lock();
	tatt = _tatt;
    roll = _r; 
    pitch = _p;
    yaw = _y;
    unlock();
  }

  void set_position(const long long _tpos, const float _lat, const float _lon, const float _alt, const float _galt)
  {
    lock();
	tpos = _tpos;
    lat = _lat;
    lon = _lon;
    alt = _alt;
	galt = _galt;
	float lat_rad = (float)(lat * (PI / 180.)), lon_rad = (float)(lon * (PI / 180.));
	getwrldrot(lat_rad, lon_rad, R);
	bihtoecef(lat_rad, lon_rad, alt, x, y, z);
    unlock();
  }

  void set_velocity(const long long & _tvel, const float _cog, const float _sog)
  {
	  lock();
	  tvel = _tvel;
	  cog = _cog;
	  sog = _sog;
	  unlock();
  }

  void set_depth(const long long & _tdp, const float _depth){
	  lock();
	  tdp = _tdp;
	  depth = _depth;
	  unlock();
  }

  void get_attitude(long long & _tatt, float & _r, float & _p, float & _y)
  {
    lock();
	_tatt = tatt;
    _r = roll;
    _p = pitch;
    _y = yaw;
    unlock();
  }

  void get_position(long long & _tpos, float & _lat, float & _lon, float & _alt, float & _galt)
  {
    lock();
	_tpos = tpos;
    _lat = lat;
    _lon = lon;
    _alt = alt;
	_galt = galt;
    unlock();
  }

  void get_position_ecef(long long & _tpos, float & _x, float & _y, float & _z)
  {
	  lock();
	  _tpos = tpos;
	  _x = x;
	  _y = y;
	  _z = z;
	  unlock();
  }

  const Mat & get_enu_rotation(long long & _tpos)
  {
	  lock();
	  _tpos = tpos;
	  R.copyTo(Rret);
	  unlock();
	  return Rret;
  }

  void get_velocity(long long & _tvel, float & _cog, float & _sog)
  {
	  lock();
	  _tvel = tvel;
	  _cog = cog;
	  _sog = sog;
	  unlock();
  }

  void get_depth(long long & _tdp, float & _depth)
  {
	  lock();
	  _tdp = tdp;
	  _depth = depth;
	  unlock();
  }

  virtual size_t get_dsize()
  {
    return sizeof(long long) * 4 + sizeof(float) * 13 + sizeof(double) * 9;
  }
  
  virtual size_t write_buf(const char *buf)
  {
    lock();
	const long long *lptr = (const long long*) buf;
	tpos = lptr[0];
	tatt = lptr[1];;
	tvel = lptr[2];
	tdp = lptr[3];

    const float * ptr = (const float*) (lptr + 4);
    roll = ptr[0];
    pitch = ptr[1];
    yaw = ptr[2];
    lat = ptr[3];
    lon = ptr[4];
    alt = ptr[5];
    galt = ptr[6];
	x = ptr[7];
	y = ptr[8];
	z = ptr[9];
    cog = ptr[10];
    sog = ptr[11];
    depth = ptr[12];

	const double * dptr = (const double*)(ptr + 13);
	memcpy((void*)R.data, (void*) dptr, sizeof(double) * 9);
    unlock();
    return get_dsize();
  }

  virtual size_t read_buf(char * buf)
  {
    lock();
	long long * lptr = (long long *) buf;
	lptr[0] = tpos;
	lptr[1] = tatt;
	lptr[2] = tvel;
	lptr[3] = tdp;
    float * ptr =  (float*) (lptr + 4);
    ptr[0] = roll;
    ptr[1] = pitch;
    ptr[2] = yaw;
    ptr[3] = lat;
    ptr[4] = lon;
    ptr[5] = alt;
    ptr[6] = galt;
	ptr[7] = x;
	ptr[8] = y;
	ptr[9] = z;
    ptr[10] = cog;
    ptr[11] = sog;
    ptr[12] = depth;
	double * dptr = (double*) (ptr + 13);
	memcpy((void*)dptr, (void*) R.data, sizeof(double) * 9);
    unlock();
    return get_dsize();
  }
  
  virtual void print(ostream & out)
  {
    out << "channel " << m_name << " RPY= " << roll << "," << pitch << "," << yaw 
	<< " pos= " << lat << "," << lon << "," << alt << endl;
  }

  virtual int write(FILE * pf, long long tcur)
  {
	  if(!pf)
		  return 0;

	  int sz = 0;
	  if(tdp <= tdpf && tvel <= tvelf && tatt <= tattf && tpos <= tposf){
		  return sz;
	  }

	  sz = sizeof(long long) * 4;

	  lock();
	  fwrite((void*) &tcur, sizeof(long long), 1, pf);

	  fwrite((void*) &tpos, sizeof(long long), 1, pf);

	  fwrite((void*) &lat, sizeof(float), 1, pf);
	  fwrite((void*) &lon, sizeof(float), 1, pf);
	  fwrite((void*) &alt, sizeof(float), 1, pf);
	  fwrite((void*) &galt, sizeof(float), 1, pf);
	  tposf = tpos;
	  sz += sizeof(float) * 4;

	  fwrite((void*) &tatt, sizeof(long long), 1, pf);
	  fwrite((void*) &roll, sizeof(float), 1, pf);
	  fwrite((void*) &pitch, sizeof(float), 1, pf);
	  fwrite((void*) &yaw, sizeof(float), 1, pf);
	  tattf = tatt;
	  sz += sizeof(float) * 3;
	  
	  fwrite((void*) &tvel, sizeof(long long), 1, pf);
	  fwrite((void*) &cog, sizeof(float), 1, pf);
	  fwrite((void*) &sog, sizeof(float), 1, pf);
	  tvelf = tvel;
	  sz += sizeof(float) * 2;
	  
	  fwrite((void*) &tdp, sizeof(float), 1, pf);
	  fwrite((void*) &depth, sizeof(float), 1, pf);
	  tdpf = tdp;
	  sz += sizeof(float);
	  m_tfile = tcur;
	  unlock();
	  return sz;
  }

  virtual int read(FILE * pf, long long tcur)
  {
	  if(!pf)
		  return 0;

	  int sz = 0;
	  if(tposf < tcur && tposf != tpos){
		  lock();
		  lat = latf;
		  lon = lonf;
		  alt = altf;
		  galt = galtf;
		  float lat_rad = (float)(lat * (PI / 180.)), lon_rad = (float)(lon * (PI / 180.));
		  getwrldrot(lat_rad, lon_rad, R);
		  bihtoecef(lat_rad, lon_rad, alt, x, y, z);
		  unlock();
		  tpos = tposf;
	  }

	  if(tattf < tcur && tattf != tatt){
		  lock();
		  roll = rollf;
		  pitch = pitchf;
		  yaw = yawf;
		  tatt = tattf;
		  unlock();
	  }

	  if(tvelf < tcur && tvelf != tvel){
		  lock();
		  cog = cogf;
		  sog = sogf;
		  tvel = tvelf;
		  unlock();
	  }

	  if(tdpf < tcur && tdpf != tdp){
		  lock();
		  depth = depthf;
		  tdp = tdpf;
		  unlock();
	  }

	  while(!feof(pf)){
		  if(m_tfile > tcur){
			  break;
		  }

		  lock();
		  size_t res = 0;
		  res += fread((void*) &m_tfile, sizeof(long long), 1, pf);

		  res += fread((void*) &tposf, sizeof(long long), 1, pf);
		  res += fread((void*) &latf, sizeof(float), 1, pf);
		  res += fread((void*) &lonf, sizeof(float), 1, pf);
		  res += fread((void*) &altf, sizeof(float), 1, pf);
		  res += fread((void*) &galtf, sizeof(float), 1, pf);			
		  sz += (int) res;

		  res += fread((void*) &tattf, sizeof(long long), 1, pf);
		  res += fread((void*) &rollf, sizeof(float), 1, pf);
		  res += fread((void*) &pitchf, sizeof(float), 1, pf);
		  res += fread((void*) &yawf, sizeof(float), 1, pf);
		  sz += (int) res;
	  
		  res += fread((void*) &tvelf, sizeof(long long), 1, pf);
		  res += fread((void*) &cogf, sizeof(float), 1, pf);
		  res += fread((void*) &sogf, sizeof(float), 1, pf);
		  sz += (int) res;

		  res += fread((void*) &tdpf, sizeof(float), 1, pf);
		  res += fread((void*) &depthf, sizeof(float), 1, pf);
		  sz += (int) res;

		  unlock();
	  }
	  return sz;
  }

  virtual bool log2txt(FILE * pbf, FILE * ptf)
  {
	  size_t sz = 0;
	  fprintf(ptf, "t, tpos, tatt, tvel, tdp, lat, lon, alt, galt, yaw, pitch, roll, cog, sog, depth\n");
	  while(!feof(pbf)){
		  long long t, tmax = 0;

		  size_t res = 0;
		  res += fread((void*) &m_tfile, sizeof(long long), 1, pbf);
		  sz += (int) res;

		  res += fread((void*) &t, sizeof(long long), 1, pbf);
		  res += fread((void*) &lat, sizeof(float), 1, pbf);
		  res += fread((void*) &lon, sizeof(float), 1, pbf);
		  res += fread((void*) &alt, sizeof(float), 1, pbf);
		  res += fread((void*) &galt, sizeof(float), 1, pbf);			
		  tposf = tpos = t;
		  sz += (int) res;

		  res += fread((void*) &t, sizeof(long long), 1, pbf);
		  res += fread((void*) &roll, sizeof(float), 1, pbf);
		  res += fread((void*) &pitch, sizeof(float), 1, pbf);
		  res += fread((void*) &yaw, sizeof(float), 1, pbf);
		  tattf = tatt = t;
		  sz += (int) res;
	  
		  res += fread((void*) &t, sizeof(long long), 1, pbf);
		  res += fread((void*) &cog, sizeof(float), 1, pbf);
		  res += fread((void*) &sog, sizeof(float), 1, pbf);

		  tvelf = tvel = t;
		  sz += (int) res;

		  res += fread((void*) &t, sizeof(float), 1, pbf);
		  res += fread((void*) &depth, sizeof(float), 1, pbf);
		  tdpf = tdp = t;
		  sz += (int) res;

		  fprintf(ptf, "%lld, %lld, %lld, %lld, %lld, %+013.8f, %+013.8f, %+06.1f, %+06.1f, %+06.1f, %+06.1f, %+06.1f, %+06.1f, %+06.1f, %+06.1f\n",
			  m_tfile, tpos, tatt, tvel, tdp, lat, lon, alt, galt, yaw, pitch, roll, cog, sog, depth);
	  }
	  return true;
  }
};

#endif
