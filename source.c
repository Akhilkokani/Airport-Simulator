/**
 * Airport Simulator
 *
 * Write a program to simulate airport traffic control for a small but busy airport assuming the following specifications. 
 * There is only one runway for both landing and takeoff. 
 * In each unit time, only one plane can land or takeoff but not both. 
 * Planes arrive and takeoff at random time, so that at any given unit, the runway may be idle or a plane may be landing or taking off. 
 * Number of planes waiting to take off or land should not exceed a certain fixed limit.
 * Landing planes should have high priority than Taking-off plane. 
 * After running for some period, the program should print statistics such as: 
 *  - number of planes processed, landed, and took off, refused; 
 *  - average landing and take-off waiting times, and average run-way idle time.
 * 
 *
 * @author Akhil Kokani
 * @package Airport Simulator
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <limits.h>

// Constants
#define MAX 3
#define ARRIVE 0
#define DEPART 1

// Type definition for plane (aeroplane)
typedef struct flyingMachine {
  
  int id;
  int tm;
} plane;

// Type deifnition for Circular Queue
typedef struct qyu {

  int count;
  int front;
  int rear;
  plane p[MAX];
} queue;

// Type definition for airport
typedef struct aerport {

  queue landing;
  queue takeoff;
  queue *pl;
  queue *pt;
  int idletime;
  int landwait, takeoffwait;
  int nland, nplanes, nrefuse, ntakeoff;
  plane pln;
} airport;

// Prototypes of functions used
void initqueue ( queue * );
void addqueue ( queue *, plane );
plane delqueue ( queue * );
int size ( queue );
int empty ( queue );
int full ( queue );
void initairport ( airport * );
void start ( int *, double *, double * );
void newplane ( airport *, int, int );
void refuse ( airport *, int );
void land ( airport *, plane, int );
void fly ( airport *, plane, int );
void idle ( airport *, int );
void conclude ( airport *, int );
int randomnumber ( double );
void apaddqueue ( airport *, char );
plane apdelqueue ( airport *, char );
int apsize ( airport, char );
int apfull ( airport, char );
int apempty ( airport, char );
void myrandomize ( );

// Used to initliase queue with its default values
void initqueue ( queue *pq ) {

  pq->count = 0;
  pq->front = 0;
  pq->rear = -1;
}

// Used to add elements in the queue
void addqueue ( queue *pq, plane item ) {

  if ( pq->count >= MAX ) {
    printf ( "\nQueue is full.\n" );
    return;
  }

  pq->count += 1;
  pq->rear = ( pq->rear + 1 ) % MAX;
  pq->p[pq->rear] = item;
}

// Used to delete element from given queue
plane delqueue ( queue *pq ) {
  
  plane p1;

  if ( pq->count <= 0 ) {

    printf ( "\nQueue is empty.\n" );
    p1.id = 0;
    p1.tm = 0;
  } 
  
  else {
    
    pq->count -= 1;
    p1 = pq->p [ pq->front ];
    pq->front = ( pq->front + 1 ) % MAX;
  }

  return p1;
}

// Determines size of given queue
int size ( queue q ) {
  return q.count;
}

// Determines whether given is empty or not
int empty ( queue q ) {
  return ( q.count <= 0 );
}

// Determines whether given queue is full or not
int full ( queue q ) {
  return ( q.count >= MAX );
}

// Initialises airport with its default values
void initairport ( airport *ap ) {

  initqueue ( &( ap-> landing ) );
  initqueue ( &( ap->takeoff ) );
  ap->pl = &( ap->landing );
  ap->pt = &( ap->takeoff );
  ap->nplanes = ap->nland = ap->ntakeoff = ap->nrefuse = 0;
  ap->landwait = ap->takeoffwait = ap->idletime = 0;
}

// Used to ask inputs from user about expected arrivals at airport and departs, 
// and seeding for random generator
void start ( int *endtime, double *expectarrive, double *expectdepart ) {
  
  int flag = 0;
  char wish;

  printf ( "\nProgram that simulates an airport with only one runway.\n" );
  printf ( "One plane can land or depart in each unit of time.\n" );
  printf ( "Up to %d planes can be waiting to land or take off at any time.\n", MAX );
  printf ( "How many units of time will the simulation run? " );
  scanf ( "%d", endtime );

  myrandomize( );

  do {

    printf ( "\nExpected number of arrivals per unit time? " );
    scanf ( "%lf", expectarrive );
    printf ( "\nExpected number of departures per unit time? " );
    scanf ( "%lf", expectdepart );

    if ( *expectarrive < 0.0 || *expectdepart < 0.0 ) {
      
      printf ( "These numbers must be nonnegative.\n" );
      flag = 0;
    } 
    else {

      if ( *expectarrive + *expectdepart > 1.0 ) {

        printf ( "The airport will become saturated. Read new numbers? " );
        scanf ( " %c", &wish );
        if ( tolower ( wish ) == 'y' )
          flag = 0;
        else
          flag = 1;
      } else flag = 1;
    }
  } while ( flag == 0 );
}

// Used to add new plane to given airport
void newplane ( airport *ap, int curtime, int action ) {
  
  ap->nplanes += 1;
  ap->pln.id = ap->nplanes;
  ap->pln.tm = curtime;

  switch ( action ) {
    case ARRIVE :
      printf ( "\n" );
      printf ( "Plane %d ready to land.\n", ap->nplanes );
      break;

    case DEPART :
      printf ( "\nPlane %d ready to take off.\n", ap->nplanes );
      break;
  }
}

// Used to refuse an plane landing in airport or departing from airport
void refuse ( airport *ap, int action ) {

  switch ( action ) {
    case ARRIVE :
      printf ( "\tplane  %d directed to another airport.\n", ap->pln.id );
      break;

    case DEPART :
      printf ( "\tplane %d told to try later.\n", ap->pln.id );
      break;
  }
  ap->nrefuse += 1;
}

// Used to land given plane in given airport
void land ( airport *ap, plane pl, int curtime ) {
  int wait;

  wait = curtime - pl.tm;
  printf ( "%d: Plane %d landed ", curtime, pl.id );
  printf ( "in queue %d units \n", wait );
  ap->nland += 1;
  ap->landwait += wait;
}

// Used to takeoff given plane in given airport
void fly ( airport *ap, plane pl, int curtime ) {
  
  int wait;
  wait = curtime - pl.tm;
  printf ( "%d: Plane %d took off ", curtime, pl.id );
  printf ( "in queue %d units \n", wait );
  ap->ntakeoff += 1;
  ap->takeoffwait += wait;
}

// Used to indicate given airport is idle
void idle ( airport *ap, int curtime ) {

  printf ( "%d: Runway is idle.\n", curtime );
  ap->idletime++;
}

// Used to display stats such as, total number of planes processed 
// and all other given in problem statement
void conclude ( airport *ap, int endtime ) {

  printf ( "\tSimulation has concluded after %d units.\n", endtime );
  printf ( "\tTotal number of planes processed: %d\n", ap->nplanes );
  printf ( "\tNumber of planes landed: %d\n", ap->nland );
  printf ( "\tNumber of planes taken off: %d\n", ap->ntakeoff );
  printf ( "\tNumber of planes refused use: %d\n", ap->nrefuse );
  printf ( "\tNumber left ready to land: %d\n", apsize ( *ap, 'l' ) );
  printf ( "\tNumber left ready to take off: %d\n", apsize ( *ap, 't' ) );

  if ( endtime > 0 )
    printf ( "\tPercentage of time runway idle: %lf \n", ( ( double ) ap->idletime / endtime ) * 100.0 );

  if ( ap->nland > 0 )
    printf ( "\tAverage wait time to land: %lf \n", ( ( double ) ap->landwait / ap->nland ) );

  if ( ap->ntakeoff > 0 )
    printf ( "\tAverage wait time to take off: %lf \n", ( ( double ) ap->takeoffwait / ap->ntakeoff ) );
}

// Used to generate number
int randomnumber ( double expectedvalue ) {
  int n = 0;
  double em;
  double x;

  em = exp ( -expectedvalue );
  x = rand( ) / ( double ) INT_MAX;

  while ( x > em ) {
    n++;
    x *= rand( ) / ( double ) INT_MAX;
  }

  return n;
}

// Used to add element in airport's queue
void apaddqueue ( airport *ap, char type ) {

  switch ( tolower( type ) ) {
    case 'l' :
      addqueue ( ap->pl, ap->pln );
      break;

    case 't' :
      addqueue ( ap->pt, ap->pln );
      break;
  }
}

// Used to delete element in airport's queue
plane apdelqueue ( airport *ap, char type ) {
  plane p1;

  switch ( tolower ( type ) ) {
    case 'l' :
      p1 = delqueue ( ap->pl );
      break;

    case 't' :
      p1 = delqueue ( ap->pl );
      break;
  }

  return p1;
}

// Used to calculate size of given airport, of given type
int apsize ( airport ap, char type ) {

  switch ( tolower ( type ) ) {
    case 'l' :
        return ( size ( *( ap.pl ) ) );

    case 't' :
        return ( size ( *( ap.pt ) ) );
  }

  return 0;
}

// Used to determine whether given type in given airport is full or not
int apfull ( airport ap, char type ) {

  switch ( tolower ( type ) ) {
    case 'l' :
        return ( full ( *( ap.pl ) ) );

    case 't' :
        return ( full ( *( ap.pt ) ) );
  }
  return 0;
}

// Used to determine whether given airport is empty or not
int apempty ( airport ap, char type ) {

  switch ( tolower(type) ) {

    case 'l' :
      return ( empty ( *( ap.pl ) ) );

    case 't' :
      return ( empty ( *( ap.pt ) ) );
  }
  return 0;
}

// Used to initialise seed for rand()
void myrandomize( ) {
  srand ( ( unsigned int ) ( time ( NULL ) % 10000 ) );
}

int main ( ) {

  airport a;
  plane temp;

  int i, pri, curtime, endtime;
  double expectarrive, expectdepart;

  // Intialising airport
  initairport ( &a );

  // Asking input from user
  start ( &endtime, &expectarrive, &expectdepart );

  // Untill units of time is complete
  for ( curtime = 1; curtime <= endtime; curtime++ ) {

    // Random number
    pri = randomnumber ( expectarrive );

    for ( i = 1; i <= pri; i++ ) {

      newplane ( &a, curtime, ARRIVE );
      if ( apfull ( a, 'l' ) )
        refuse ( &a, ARRIVE );
      else
        apaddqueue( &a, 'l' );
    }

    pri = randomnumber ( expectdepart );

    for ( i = 1; i <= pri; i++ ) {

      newplane ( &a, curtime, DEPART );
      if ( apfull ( a, 't' ) )
        refuse ( &a, DEPART );
      else
        apaddqueue ( &a, 't' );
    }

    if (  ! ( apempty ( a, 'l' ) ) ) {

      temp = apdelqueue ( &a, 'l' );
      land ( &a, temp, curtime );
    } else {

      if ( ! ( apempty ( a, 't' ) ) ) {

        temp = apdelqueue ( &a, 't' );
        fly ( &a, temp, curtime );
      }
      else
        idle ( &a, curtime );
    }
  }

  // Conclude by showing stats to user.
  conclude ( &a, endtime );
  return 0;
}
