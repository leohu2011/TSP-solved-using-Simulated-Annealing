#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <list>
#include <sstream>
#include <cstdio>
#include <map>
#include <cstdlib>
#include <cassert>
#include <string>
#include <cmath>

using namespace std;


//map information, city name and its coordinates
map< char, pair<int, int> > mapinfo;

//the sequence of visiting all cities
vector<char> sequence;

//size if instance
int size_of_map;


//distance btw cities, implemented as a two dimentional array for fast access
#define row 36
#define col 36
double dist[row][col];

//read in the name of the file
char* filename;

//int counter = 20;


void print();

//read in the map information
void construct_map(){
   char name;
   int x;
   int y;
   
   pair<int, int> temp;
   
   ifstream infile;
   //setting the name of the file
   infile.open( filename );
   
   infile >> size_of_map;
   
   while(!infile.eof()){
      infile >> name >> x >> y;
      temp = make_pair(x, y);
      mapinfo.insert (pair<char, pair<int, int> >(name, temp) );
   }
   
   for (map<char , pair<int, int> >::iterator it = mapinfo.begin(); it != mapinfo.end(); ++it){
      sequence.push_back(it->first);
   }
}


//calculate the distance btw any two cities
void create_distance(){
   int size = mapinfo.size();
   for (map<char , pair<int, int> >::iterator it = mapinfo.begin(); it != mapinfo.end(); ++it){
      for (map<char , pair<int, int> >::iterator it2=mapinfo.begin(); it2!=mapinfo.end(); ++it2){
         int x_diff = it->second.first - it2->second.first;
         int y_diff = it->second.second - it2->second.second;
         double diff = sqrt(pow( x_diff , 2) + pow( y_diff, 2) );
         dist[it->first - 'A'][it2->first - 'A'] = diff;
      }
   }
}

//probability function that return e^(delta_v/temperature)
double probability(double delta_v, int temperature){
   return exp(delta_v/temperature);
}



//get the new distance after swapping a and b
double calculate_new_dist(double current, int a, int b){
   char first = sequence[a];
   char second = sequence[b];
   
   char before1, after1, before2, after2;
   
   
   //the following clauses includes all conditions where the swapped choices are neighbors or further apart,
   //if any one of swpa choices are the last city
   if (abs(a - b) == 1){
      int front = a<b ? a : b;
      int back = a>b ? a : b;
      char before = sequence[front - 1];
      char after;
      if (back == size_of_map -1) {
         after = sequence[0];
      }
      else after = sequence[back + 1];
      
      //cout << "front and back are: " << sequence[front] << " and " << sequence[back] << endl;
      //cout << "before and after are: " << before << " and " << after << endl;
      
      double value = current - dist[sequence[front] - 'A'][before - 'A'] - dist[sequence[back] - 'A'][after - 'A'] + dist[sequence[back] - 'A'][before-'A'] + dist[sequence[front] - 'A'][after-'A'];
      
      //cout << dist[sequence[front] - 'A'][before - 'A'] << endl;
      //cout << dist[sequence[back]][after - 'A'] << endl;
      //cout << dist[sequence[back]][before-'A'] << endl;
      //cout << dist[sequence[front]][after-'A'] << endl;
      
      return value;
   }
   
   else if (a == size_of_map - 1){
       before1 = sequence[ a - 1];
       after1 = sequence[0];
       before2 = sequence[ b - 1];
       after2 = sequence[ b + 1];
   }
   
   else if (b == size_of_map - 1){
       before1 = sequence[ a - 1];
       after1 = sequence[ a + 1 ];
       before2 = sequence[ b - 1];
       after2 = sequence[0];
   }
   
   else {
       before1 = sequence[ a - 1];
       after1 = sequence[ a + 1 ];
       before2 = sequence[ b - 1];
       after2 = sequence[ b + 1];
   }
   
   double temp = dist[before1-'A'][first-'A'] + dist[after1-'A'][first-'A'];
   double temp2 = dist[before2-'A'][second-'A'] + dist[after2-'A'][second-'A'];
   
   double temp3 = dist[before1-'A'][second-'A'] + dist[after1-'A'][second-'A'];
   double temp4 = dist[before2-'A'][first-'A'] + dist[after2-'A'][first-'A'];
   
   //cout << before1 << " " << after1 << " " << first << endl;
   //cout << before2 << " " << after2 << " " << second << endl;
   //cout << temp << "  " << temp2 << "  " << temp3 << "  " << temp4 << endl;
   
   
   //new distance is equal to current distance minus the existing 4 connections between the two chosen cities then plus the newly added 4 connections after the two cities are swapped
   double next = current - temp - temp2 + temp3 + temp4;
   
   return next;
}
   
//calculate the initial distance
double calculate_init_dist(){

   double overall = 0;
   
   //is there is only one city, overall distance is 0
   if (size_of_map == 1) return 0;
   
   
   
   for (int i = 0; i < sequence.size() - 1; i++){
      //cout<< "entering loop" << endl;
      char current = sequence[i];
      char next = sequence[i + 1];
      double step = dist[current-'A'][next-'A'];
      overall += step;
   }
   
   //adding the back to start point
   overall += dist[ sequence[0] - 'A'][ sequence[sequence.size()-1] - 'A'];
   
   return overall;
}

//return the greater of a or b
int max(int a, int b){
   return a<b ? b : a;
}

//recursion step
double recursion(double initial_dist){
   double current_dist = initial_dist;

   int temp = size_of_map - 1;
   
   //set initial temperature 
   int temperature = 1000;
   int rate = temperature / 100;
   
   while (temperature >= 1){
      //cout << "temperature is: " << temperature << endl;
      
      
      //generate two random numbers that is not zero
      //srand(time(NULL));
      int random1 = ( rand() % temp ) + 1;
   
      //srand(time(NULL));
      int random2 = ( rand() % temp ) + 1;
      double prob;
      while (random1 == random2) {
         random2 = ( rand() % temp ) + 1;
      }
      
      //double random3 = ( rand() % 100 ) + 1;
      //cout << "random3 is: " << random3 << endl;
      
      double new_dist;
      new_dist = calculate_new_dist(current_dist, random1, random2);
      
      //probability is considered only when the alternative is suboptimal
      if (new_dist > current_dist){
         prob = probability( current_dist - new_dist, temperature);
      }
      
      //cout<< "current dist is: "<< current_dist << endl;
      //cout << "new dist is: " << new_dist << endl;
      
      double prob2 = (rand()/(double)(RAND_MAX));
      //cout << "prob2 is: " << prob2 << endl;
      //cout << "prob is: " << prob << endl;

      //if the alternative is better
      if (new_dist < current_dist){
         //cout << "found a better place" << endl;
         current_dist = new_dist;
         char copy = sequence[random1];
         sequence[random1] = sequence[random2];
         sequence[random2] = copy;
      }
      
      //if prob is true then go to suboptimal position
      else if (prob > prob2) {
         //cout << "probability is: " << prob << " greater than random number: " << prob2 << endl;
         //cout << "a bit worse but lets explore it" << endl;
         current_dist = new_dist;
         char copy = sequence[random1];
         sequence[random1] = sequence[random2];
         sequence[random2] = copy;
      }
      
      
      //if prob is false then just stay put
      else {
        //cout << "I am just gonna stay put" << endl;
      }
      
      //cout << "swap choices are: " <<  sequence[random1] <<  " and " << sequence[random2] << endl;
      //print();
      //cout << "new overall distance is: " << current_dist << endl;
      
      
      
      
      //annealing schedules, how temperature is decremented
      temperature --;
      //temperature = 0.99 * temperature;
      //temperature = temperature - max(rate, 1);
      //rate = rate * 0.999;
      //cout << "-------------------------------------------------" << endl;
      
      //counter --;
   }
   
   return current_dist;
}
   

void print(){
   for (int i = 0; i < size_of_map -1; i++){
      char current = sequence[i];
      char next = sequence[i + 1];
      double step = dist[current-'A'][next-'A'];
      //cout << current << " --> " << next << "  has distance: " << step << endl;
   }
   //cout << "returning step is: " << sequence[size_of_map - 1] << " --> " << sequence[0] << " with distance: " << dist[ sequence[0] - 'A'][ sequence[sequence.size()-1] - 'A'] << endl;

   
}

//shuffle/randomize the initial solution
void shuffle(){
   int random = (rand() % (size_of_map - 1) ) + 1;
   //char* temp = sequence.begin()+ 1;
   random_shuffle(sequence.begin()+1, sequence.end());
}




int main(int argc, char** argv){

	if(argc > 1){
		filename = argv[1];
	}
	
   construct_map();
   
   create_distance();
   
   //cout << "size of map is: " << size_of_map << endl;
   //cout << "size of sequence is: " << sequence.size() << endl;
   
   shuffle();
   
   double initial_dist;
   initial_dist = calculate_init_dist();
   cout<< "initial distance is: "<< initial_dist << endl;
   //print();
   
   double result_dist;
   //new_dist = calculate_new_dist(initial_dist, 1, 2);
   //cout<< "new distance is: " << new_dist << endl;
   
   
   result_dist = recursion(initial_dist);
   
   print();
   cout << "final overall distance is: " << result_dist << endl;
   

   
   //printpath(ans);
	
   //cout << endl;
   //cout << dist['D' - 'A']['C' - 'A'] << endl;
   return 1;
}
