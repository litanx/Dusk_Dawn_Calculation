#include "main.h"
#include "math.h"

#define PI 3.141592

/* Private functions ------------------------------------------------------*/
uint32_t gregorian_calendar_to_jd(uint32_t year, uint8_t month, uint8_t day);
void getDuskDawn( RTC_DateTypeDef date, float lati, float longi, double *Dusk, double *Dawn);



/* Exported functions ------------------------------------------------------*/
double calcDuskUTC(RTC_DateTypeDef date, double latitude, double longitude){

  double Dusk;
  double Dawn;

  getDuskDawn(date, latitude, longitude, &Dusk, &Dawn);

  return Dusk;

}

double calcDawnUTC(RTC_DateTypeDef date, double latitude, double longitude){
 
  double Dusk;
  double Dawn;

  getDuskDawn(date, latitude, longitude, &Dusk, &Dawn);

  return Dawn;
}

void getDuskDawn( RTC_DateTypeDef date, float lati, float longi, double *Dusk, double *Dawn){

  /* Source: http://users.electromagnetic.net/bu/astro/sunrise-set.php */

  uint32_t JulianDate = gregorian_calendar_to_jd(date.Year+2000, date.Month , date.Date );

  int nStarRnd = round((JulianDate-2451545-0.0009)+(longi/360));
  double JStar = 2451545.0009-(longi/360)+nStarRnd;

  double MeanAnom0 = fmod( (357.5291+0.98560028*(JStar-2451545)), 360 );
  double EqnOfCentre0 = (1.9148*sin(MeanAnom0*PI/180))+(0.02*sin(2*MeanAnom0*PI/180))+(0.0003*sin(3*MeanAnom0*PI/180));
  double EclipLongSun0 = fmod((MeanAnom0+EqnOfCentre0+282.9372),360);
  double JTrans0 =JStar+(0.0053*sin(MeanAnom0*PI/180))-(0.0069*sin(2*EclipLongSun0*PI/180));
 
  double MeanAnom1 =fmod((357.5291+0.98560028*(JTrans0-2451545)),360);
  double EqnOfCentre1 =(1.9148*sin(MeanAnom1*PI/180))+(0.02*sin(2*MeanAnom1*PI/180))+(0.0003*sin(3*MeanAnom1*PI/180));
  double EclipLongSun1 =fmod((MeanAnom1+EqnOfCentre1+282.9372),360);
  double JTrans1 =JStar+(0.0053*sin(MeanAnom1*PI/180))-(0.0069*sin(2*EclipLongSun1*PI/180));
 
  double MeanAnom2 =fmod((357.5291+0.98560028*(JTrans1-2451545)),360);
  double EqnOfCentre2 =(1.9148*sin(MeanAnom2*PI/180))+(0.02*sin(2*MeanAnom2*PI/180))+(0.0003*sin(3*MeanAnom2*PI/180));
  double EclipLongSun2 =fmod((MeanAnom2+EqnOfCentre2+282.9372),360);
  double JTrans2 =JStar+(0.0053*sin(MeanAnom2*PI/180))-(0.0069*sin(2*EclipLongSun2*PI/180));
 
  double DeclinSun = asin((sin(EclipLongSun2*PI/180)*sin(23.45*PI/180)))*180/PI;
  double HourAngle = acos(((sin(-0.83*PI/180)-sin(lati*PI/180)*sin(DeclinSun*PI/180))/(cos(lati*PI/180)*cos(DeclinSun*PI/180))))*180/PI;
 
  double JStarStar =2451545.0009+((HourAngle-longi)/360)+nStarRnd;
 
  double JSunSet =JStarStar+(0.0053*sin(MeanAnom2*PI/180)-(0.0069*sin(2*EclipLongSun2*PI/180)));
  double JSunRise =JTrans2-(JSunSet-JTrans2);  


  int duskH = (int)( fmod((JSunSet+0.5) , 1) * 24 );
  int duskM = (int) ( fmod((fmod((JSunSet+0.5) , 1) * 24), 1) * 60 );
    
  int dawnH = (int)( fmod((JSunRise+0.5) , 1) * 24 );
  int dawnM = (int) ( fmod((fmod((JSunRise+0.5) , 1) * 24), 1) * 60 );
    
  *Dawn =  fmod((JSunRise+0.5) , 1) * 1440;  // Dawn in minutes since 00:00:00
  *Dusk =  fmod((JSunSet+0.5) , 1) * 1440;  // Dusk in minutes since 00:00:00

}

//Function to take date of installation and convert it to a julian date
uint32_t gregorian_calendar_to_jd(uint32_t year, uint8_t month, uint8_t day)
    {
    uint32_t y = year;
    uint32_t m = month;
    uint32_t d = day;
    y+=8000;
    if(m<3) { y--; m+=12; }

    return (y*365) + (y/4) -(y/100) +(y/400) -1200820 +(m*153+3)/5-92 +d-1;
    }
