#ifndef _CH_OBJ_H_
#define _CH_OBJ_H_
// Copyright(c) 2016 Yohei Matsumoto, All right reserved. 

// ch_obj.h is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ch_obj.h is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ch_obj.h.  If not, see <http://www.gnu.org/licenses/>.

#include "ch_base.h"

// Object source (Defines how the object is detected.)
enum e_obj_src
{
	 EOS_UNDEF = 0, EOS_VMAN=0x1, EOS_VAUTO=0x2, EOS_AIS=0x4
};

// Object state (How the object is treated in the filters observing)
enum e_obj_trck_state
{
	EOST_UNDEF = 0, EOST_TRCK=0x1, EOST_DET=0x2, EOST_LOST=0x4, EOST_NOINT=0x8
};

// Object type (The type of the object, corresponding to the sub class)
enum e_obj_type
{
	EOT_UNKNOWN = 0, EOT_SHIP
};

// Data related to the object
enum e_obj_data_type
{
	EOD_UNDEF=0, EOD_IMG=0x1, EOD_IMRECT=0x2, EOD_MDL3D=0x4, 
	EOD_POS_BIH=0x8, EOD_VEL_BIH = 0x10, 
	EOD_POS_ECEF=0x20, EOD_VEL_ECEF = 0x40,
	EOD_POS_REL=0x80, EOD_VEL_REL = 0x100,
	EOD_ATTD=0x200, EOD_AIS=0x400
};

// The base class of the objects in aws
class c_obj
{
protected:
	e_obj_type m_type;			// Type of the object
	e_obj_src m_src;			// Source sensor of the object information
	e_obj_trck_state m_tst;		// Tracking status.
	e_obj_data_type m_dtype;	// Data types the object hold

	long long m_t;				// Time updated.
	float m_lat, m_lon, m_alt;	// BIH coordinate
	float m_cog, m_sog;			// Velocity in BIH coordinate
	float m_x, m_y, m_z;		// ECEF  coordinate
	float m_vx, m_vy, m_vz;		// Velocity in ECEF
	float m_xr, m_yr, m_zr;		// Relative orthogonal coordinate (centered at my own ship)
	float m_vxr, m_vyr, m_vzr;	// Reltative velocity in orthogonal coordinate centered at my own ship
public:
	c_obj();
	virtual ~c_obj();

	void set(const e_obj_type & type){
		m_type = type;
	}

	const e_obj_type & get_type(){
		return m_type;
	}

	void set(e_obj_src & src){
		m_src = src;
	}

	const e_obj_src & get_src(){
		return m_src;
	}

	void set(const e_obj_trck_state & tst){
		m_tst = tst;
	}

	const e_obj_trck_state & get_tst(){
		return m_tst;
	}

	void set(const e_obj_data_type & dtype){
		m_dtype = dtype;
	}

	const e_obj_data_type  & get_dtype(){
		return m_dtype;
	}

	void set_time(const long long t){
		m_t = t;
	}

	void set_bih(const float lat, const float lon, const float alt){
		m_lat = lat;
		m_lon = lon;
		m_alt = alt;
		m_dtype = (e_obj_data_type)(m_dtype | EOD_POS_BIH);
	}

	void reset_bih(){
		m_dtype = (e_obj_data_type)(m_dtype & ~EOD_POS_BIH);
	}

	void get_bih(float & lat, float & lon, float & alt){
		lat = m_lat;
		lon = m_lon;
		alt = m_alt;
	}

	void set_vel_bih(const float cog, const float sog){
		m_cog = cog;
		m_sog = sog;
		m_dtype = (e_obj_data_type)(m_dtype | EOD_VEL_BIH);
	}

	void reset_vel_bih(){
		m_dtype = (e_obj_data_type)(m_dtype & ~EOD_VEL_BIH);
	}

	void get_vel_bih(float & cog, float & sog){
		cog = m_cog;
		sog = m_sog;
	}

	void set_ecef(const float x, const float y, const float z){
		m_x = x;
		m_y = y;
		m_z = z;
		m_dtype = (e_obj_data_type)(m_dtype | EOD_POS_ECEF);
	}

	void reset_ecef(){
		m_dtype = (e_obj_data_type)(m_dtype & ~EOD_POS_ECEF);
	}

	void get_ecef(float & x, float & y, float & z){
		x = m_x;
		y = m_y;
		z = m_z;
	}

	void set_vel_ecef(const float vx, const float vy, const float vz){
		m_vx = vx;
		m_vy = vy;
		m_vz = vz;
		m_dtype = (e_obj_data_type)(m_dtype | EOD_VEL_ECEF);
	}

	void reset_vel_ecef(){
		m_dtype = (e_obj_data_type)(m_dtype & ~EOD_VEL_ECEF);
	}

	void get_vel_ecef(float & vx, float & vy, float & vz){
		vx = m_vx;
		vy = m_vy;
		vz = m_vz;
	}

	void set_pos_rel(const float xr, const float yr, const float zr){
		m_xr = xr;
		m_yr = yr;
		m_zr = zr;
		m_dtype = (e_obj_data_type)(m_dtype | EOD_POS_REL);
	}

	void reset_pos_rel(){
		m_dtype = (e_obj_data_type)(m_dtype & ~EOD_POS_REL);
	}

	void get_pos_rel(float & xr, float & yr, float & zr){
		xr = m_xr;
		yr = m_yr;
		zr = m_zr;
	}

	void set_vel_rel(const float & vxr, const float & vyr, const float & vzr){
		m_vxr = vxr;
		m_vyr = vyr;
		m_vzr = vzr;
		m_dtype = (e_obj_data_type)(m_dtype | EOD_VEL_REL);
	}

	void reset_vel_rel(){
		m_dtype = (e_obj_data_type)(m_dtype & ~EOD_VEL_REL);
	}

	void get_vel_rel(float & vxr, float & vyr, float & vzr){
		vxr = m_vxr;
		vyr = m_vyr;
		vzr = m_vzr;
	}
};


// Visual object (Rectangle and template image can be stored.)
class c_vobj: public c_obj
{
protected:
	struct s_rc{
		long long t;
		Rect rc;		// rectangle in the image
		Mat tmp;	    // template image
		const f_base * pfsrc; // Image source filter.
		s_rc():t(0), rc(0,0,0,0), tmp(), pfsrc(NULL){
		}
	} m_rc;


public:
	c_vobj();
	c_vobj(const long long t, const Rect & rc, const Mat & tmp, const f_base * pfsrc, bool manual=false);

	void set(const long long t, const Rect & rc, const Mat & tmp, const f_base * pfsrc, bool manual=false)
	{
		m_dtype = EOD_IMRECT;
		m_rc.t = t;
		m_rc.pfsrc = pfsrc;
		m_rc.rc = rc;
		m_rc.tmp = tmp.clone();

		if(!m_rc.tmp.empty()){
			m_dtype =(e_obj_data_type) (m_dtype | EOD_IMG);
		}

		if(manual){
			m_src = EOS_VMAN;
		}else{
			m_src = EOS_VAUTO;
		}
	}

	virtual ~c_vobj();
};

// AIS object
class c_ais_obj: public c_obj
{
protected:
	unsigned int m_mmsi;
public:
	c_ais_obj();
	c_ais_obj(const long long t, const unsigned int mmsi, float lat, float lon, float cog, float sog);
	virtual ~c_ais_obj();

	void set(const long long t, const unsigned int mmsi, float lat, float lon, float cog, float sog)
	{
		m_type = EOT_SHIP;
		m_src = EOS_AIS;
		if(m_tst == EOST_DET || m_tst == EOST_TRCK)
			m_tst = EOST_TRCK;
		m_dtype= EOD_AIS;

		set_time(t);
		m_mmsi = mmsi;
		set_bih(lat, lon, 0.);
		set_vel_bih(cog, sog);
	}

	void update(const long long t, float lat, float lon, float cog, float sog){
		m_dtype = EOD_AIS;
		set_bih(lat, lon, 0.);
		set_vel_bih(cog, sog);
	}
};

// contains recent object list, expected object list
// has insert, delete, and search method
class ch_obj: public ch_base
{
protected:
	list<c_obj*> objs;
	list<c_obj*>::iterator itr;
public:
	ch_obj(const char * name): ch_base(name)
	{
		itr = objs.begin();
	}

	virtual ~ch_obj()
	{
	}

	void ins(c_obj * pobj){
		itr = objs.insert(itr, pobj);
	}

	void ers(){
		itr = objs.erase(itr);
	}
	
	c_obj * cur(){
		return *itr;
	}

	bool is_end(){
		return itr == objs.end();
	}

	bool is_begin(){
		return itr == objs.begin();
	}

	c_obj * begin(){
		return *(itr = objs.begin());
	}

	void end(){
		itr = objs.end();
	}

	void next(){
		if(objs.end() != itr)
			itr++;
	}

	void prev(){
		if(objs.begin() != itr)
			itr--;
	}

	int get_num_objs(){
		objs.size();
	}
};

#endif
