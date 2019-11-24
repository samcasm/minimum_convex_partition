
/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <json-c/json.h>
#include <bits/stdc++.h>
#include "helpers.h"

using namespace std;

Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
unsigned int display_width, display_height;

Window win;
int border_width;
unsigned int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Example Window";
char *icon_name_string = "Icon for Example Window";

XEvent report;

GC gc, gc_yellow, gc_red, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_yellow_values, gc_red_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

int main(int argc, char **argv)
{
  FILE *fp;
  char buffer[4000];
  struct json_object *parsed_json;
  struct json_object *points;
  struct json_object *point;
  struct json_object *x;
  struct json_object *y;

  size_t n_points;
  size_t n_point;

  size_t i;

  std::vector<Point> points_vector;

  fp = fopen("test1.json", "r");
  fread(buffer, 4000, 1, fp);
  fclose(fp);

  parsed_json = json_tokener_parse(buffer);

  json_object_object_get_ex(parsed_json, "points", &points);

  // printf("Name: %s\n", json_object_get_string(name));
  // printf("Age: %d\n", json_object_get_int(age));

  n_points = json_object_array_length(points);
  printf("Found %lu points\n", n_points);

  for (i = 0; i < n_points; i++)
  {
    point = json_object_array_get_idx(points, i);
    json_object_object_get_ex(point, "x", &x);
    json_object_object_get_ex(point, "y", &y);
    struct Point temp_point = Point({json_object_get_int(x), json_object_get_int(y)});
    points_vector.push_back(temp_point);
  }
  cout << points_vector.size() << " points\n";

  float min_x = INT_MAX;
  float min_y = INT_MAX;
  float max_x = INT_MIN;
  float max_y = INT_MIN;

  for (i = 0; i < points_vector.size(); i++)
  {

    if (min_x > points_vector[i].x)
    {
      min_x = points_vector[i].x - 500;
    }
    if (min_y > points_vector[i].y)
    {
      min_y = points_vector[i].y - 500;
    }
    if (max_x < points_vector[i].x)
    {
      max_x = points_vector[i].x + 500;
    }
    if (max_y < points_vector[i].y)
    {
      max_y = points_vector[i].y + 500;
    }
  }

  cout << min_x << "min x \n"
       << min_y << "min_y \n"
       << max_x << "max_x\n"
       << max_y << "max_y\n";

  stack<Point> S = convexHull(points_vector, points_vector.size());

  float x_scalefactor;
  float y_scalefactor;
  Point curr_point;

  struct LineEndPoint{
	  Point startPoint;
	  Point endPoint;
  };
  std::vector<LineEndPoint> P;
  
  cout << S.size() << " the stack size\n";
  struct LineEndPoint l;
  struct Point firstPoint = S.top();
  struct Point lastPoint;

  while(!S.empty()){
		  
		  curr_point = S.top();
		  S.pop();
		  
		  if (S.size() == 0){
			  lastPoint = curr_point;
			  break;
		  }
		  else if(S.size() == 1){
			  lastPoint = S.top();
		  }
		  l = LineEndPoint({curr_point, S.top()});
		  P.push_back(LineEndPoint({curr_point, S.top()}));
	  }
  P.push_back(LineEndPoint({firstPoint, lastPoint}));
  
  
  /********** display begins here  **********/

  /* opening display: basic connection to X Server */
  if ((display_ptr = XOpenDisplay(display_name)) == NULL)
  {
    printf("Could not open display. \n");
    exit(-1);
  }
  printf("Connected to X server  %s\n", XDisplayName(display_name));
  screen_num = DefaultScreen(display_ptr);
  screen_ptr = DefaultScreenOfDisplay(display_ptr);
  color_map = XDefaultColormap(display_ptr, screen_num);
  display_width = DisplayWidth(display_ptr, screen_num);
  display_height = DisplayHeight(display_ptr, screen_num);

  printf("Width %d, Height %d, Screen Number %d\n",
         display_width, display_height, screen_num);
  /* creating the window */
  border_width = 10;
  win_x = 0;
  win_y = 0;
  win_width = display_width / 2;
  win_height = (int)(win_width / 1.7); /*rectangular window*/
  
  x_scalefactor = (float(win_width) / float(max_x - min_x));
  y_scalefactor = (float(win_height) / float(max_y - min_y));

  win = XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num),
                            win_x, win_y, win_width, win_height, border_width,
                            BlackPixel(display_ptr, screen_num),
                            WhitePixel(display_ptr, screen_num));
  /* now try to put it on screen, this needs cooperation of window manager */
  size_hints = XAllocSizeHints();
  wm_hints = XAllocWMHints();
  class_hints = XAllocClassHint();
  if (size_hints == NULL || wm_hints == NULL || class_hints == NULL)
  {
    printf("Error allocating memory for hints. \n");
    exit(-1);
  }

  size_hints->flags = PPosition | PSize | PMinSize;
  size_hints->min_width = 60;
  size_hints->min_height = 60;

  XStringListToTextProperty(&win_name_string, 1, &win_name);
  XStringListToTextProperty(&icon_name_string, 1, &icon_name);

  wm_hints->flags = StateHint | InputHint;
  wm_hints->initial_state = NormalState;
  wm_hints->input = False;

  class_hints->res_name = "x_use_example";
  class_hints->res_class = "examples";

  XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc,
                   size_hints, wm_hints, class_hints);

  /* what events do we want to receive */
  XSelectInput(display_ptr, win,
               ExposureMask | StructureNotifyMask | ButtonPressMask);

  /* finally: put window on screen */
  XMapWindow(display_ptr, win);

  XFlush(display_ptr);

  /* create graphics context, so that we may draw in this window */
  gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
  XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
  XSetLineAttributes(display_ptr, gc, 4, LineSolid, CapRound, JoinRound);
  /* and three other graphics contexts, to draw in yellow and red and grey*/
  gc_yellow = XCreateGC(display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow, 6, LineSolid, CapRound, JoinRound);
  if (XAllocNamedColor(display_ptr, color_map, "yellow",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color yellow\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
  gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
  XSetLineAttributes(display_ptr, gc_red, 6, LineSolid, CapRound, JoinRound);
  if (XAllocNamedColor(display_ptr, color_map, "red",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color red\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
  gc_grey = XCreateGC(display_ptr, win, valuemask, &gc_grey_values);
  if (XAllocNamedColor(display_ptr, color_map, "light grey",
                       &tmp_color1, &tmp_color2) == 0)
  {
    printf("failed to get color grey\n");
    exit(-1);
  }
  else
    XSetForeground(display_ptr, gc_grey, tmp_color1.pixel);

  XFillArc(display_ptr, win, gc_red,
           500, 500,
           win_height / 50, win_height / 50, 0, 360 * 64);
  
 
    // while(!S.empty()){
		  
	// 	  curr_point = S.top();
	// 	  S.pop();
	// 	  XDrawLine(display_ptr, win, gc_red, x_scalefactor * (curr_point.x - min_x), y_scalefactor * (curr_point.y - min_y),
    //                x_scalefactor * (S.top().x - min_x), y_scalefactor * (S.top().y - min_y) );
		  
	//   }

  /* and now it starts: the event loop */
  while (1)
  {
    XNextEvent(display_ptr, &report);
    switch (report.type)
    {
    case Expose:
      /* (re-)draw the example figure. This event happens
             each time some part ofthe window gets exposed (becomes visible) */
     
	 
      for (int i = 0; i < points_vector.size(); i++)
      {
        XFillArc(display_ptr, win, gc,
                 x_scalefactor * (points_vector[i].x - min_x), y_scalefactor * (points_vector[i].y - min_y),
                 win_height / 100, win_height / 100, 0, 360 * 64);
      }
	  
	  for (int i=0; i<P.size();i++){
		  XDrawLine(display_ptr, win, gc, x_scalefactor * (P[i].startPoint.x - min_x), y_scalefactor * (P[i].startPoint.y - min_y),
                   y_scalefactor * (P[i].endPoint.x - min_x), y_scalefactor * (P[i].endPoint.y - min_y) );
	  }
	  
	//   curr_point = S.top();
	//   S.pop();
	//   XDrawLine(display_ptr, win, gc_red, x_scalefactor * (curr_point.x - min_x), y_scalefactor * (curr_point.y - min_y),
    //                y_scalefactor * (S.top().x - min_x), y_scalefactor * (S.top().y - min_y) );
	//   S.top();
	//   while(!S.empty()){
		  
	// 	  curr_point = S.top();
	// 	  S.pop();
	// 	  XDrawLine(display_ptr, win, gc_red, x_scalefactor * (curr_point.x - min_x), y_scalefactor * (curr_point.y - min_y),
    //                y_scalefactor * (S.top().x - min_x), y_scalefactor * (S.top().y - min_y) );
		  
	//   }
	
	
  

      break;
    case ConfigureNotify:
      /* This event happens when the user changes the size of the window*/
      win_width = report.xconfigure.width;
      win_height = report.xconfigure.height;



      break;
    case ButtonPress:
      /* This event happens when the user pushes a mouse button. I draw
            a circle to show the point where it happened, but do not save 
            the position; so when the next redraw event comes, these circles
      disappear again. */
      {
        int x, y;
        x = report.xbutton.x;
        y = report.xbutton.y;
        if (report.xbutton.button == Button1)
          XFillArc(display_ptr, win, gc_red,
                   x - win_height / 40, y - win_height / 40,
                   win_height / 20, win_height / 20, 0, 360 * 64);
        else
          XFillArc(display_ptr, win, gc_yellow,
                   x - win_height / 40, y - win_height / 40,
                   win_height / 20, win_height / 20, 0, 360 * 64);
      }
      break;
    default:
      /* this is a catch-all for other events; it does not do anything.
             One could look at the report type to see what the event was */
      break;
    }
  }
  exit(0);
}

