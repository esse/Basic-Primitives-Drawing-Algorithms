#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/time.h>
 
#define swap(a,b)           {a^=b; b^=a; a^=b;}

const int SCREEN_W = 800;
const int SCREEN_H = 600;
const int BOUNCER_SIZE = 92;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *bmp = NULL;

ALLEGRO_COLOR black;

unsigned char pix_r;
unsigned char pix_g;
unsigned char pix_b;

// realizacja kolejki

struct queue_node
{
  struct queue_node *next;     
  int data;
};

struct queue
{
  struct queue_node *first;
  struct queue_node *last;
};

int enqueue(struct queue *q, const int value)
{
  struct queue_node *node = malloc(sizeof(struct queue_node));
  if (node == NULL) {
    errno = ENOMEM;
    return 1;
  }
  node->data = value;
  if (q->first == NULL) {
    q->first = q->last = node;
  } else {
    q->last->next = node;
    q->last = node;
  }
  node->next = NULL;
  return 0;
}

int dequeue(struct queue *q, int *value)
{
  if (!q->first) {
    *value = 0;
    return 1;
  }
  *value = q->first->data;
  struct queue_node *tmp = q->first;
  q->first = q->first->next;
  free(tmp);
  return 0;
}

void init_queue(struct queue *q)
{
  q->first = q->last = NULL;
}

int queue_empty_p(const struct queue *q)
{
  return q->first == NULL;
}

void plot(int x, int y, int flag)
{
	if (flag)
		al_put_pixel(y, x, black);
	else
   al_put_pixel(x, y, black);
}

void clear_screen()
{
  al_set_target_bitmap(al_get_backbuffer(display));
  al_clear_to_color(al_map_rgb(255, 255, 255));
  al_flip_display();
  al_clear_to_color(al_map_rgb(255, 255, 255));
  al_set_target_bitmap(al_get_backbuffer(display)); 
}

void przyrostowy(int x0, int y0, int x1, int y1)
{
	if (abs(x0-x1) < abs(y0-y1))
	{
		if (y0 > y1)
		{
			int x_temp;
			int y_temp;
			x_temp = x0;
			y_temp = y0;
			x0 = x1;
			y0 = y1;
			x1 = x_temp;
			y1 = y_temp;
		}
		float dy, dx, m;
		int y;
		float x;
		al_put_pixel(x0, y0, black);
		dy = y1 - y0;
		dx = x1 - x0;
		m = dx / dy;
		x = x0;
		for (y = y0; y <= y1; y++)
		{
			al_put_pixel((int)floor(x+0.5), y, black);
			x += m;
		}
	}
	else
	{
		if (x0 > x1)
		{
			int x_temp;
			int y_temp;
			x_temp = x0;
			y_temp = y0;
			x0 = x1;
			y0 = y1;
			x1 = x_temp;
			y1 = y_temp;
		}
		float dy, dx, m;
		float y;
		int x;
		al_put_pixel(x0, y0, black);
		dy = y1 - y0;
		dx = x1 - x0;
		m = dy / dx;
		y = y0;
		for (x = x0; x <= x1; x=x+1)
		{
			al_put_pixel(x, (int)floor(y+0.5), black);
			y += m;
		}
	}	
	
}

void wurokne(int a1, int b1, int a2, int b2)
{
	int dx; int dy; int incr1; int incr2; int D; int x; int y; int xend; int c; int parity;
  int reverse = 0;
  dx = a2-a1; dy = b2-b1;
  int sign_x = dx < 0 ? -1 : 1;
  int sign_y = dy < 0 ? -1 : 1;
  if ((int)abs(dy) > (int)abs(dx)) {
		swap(a1, b1);
		swap(a2, b2);
    swap(dx, dy);
		reverse = 1;
	}
  dx = (int)abs(a2-a1);
  dy = (int)abs(b2-b1);
  if (a1 > a2)
  {
    swap(a1, a2);
    swap(b1, b2);
  }
  int step;
  if (sign_x == sign_y)
  {
  step = 1;
  } else {
    step = -1;
  }
  x=a1; y=b1;
  if (dx%2 == 0)
  {
    parity = 0;
    xend = a2;
  } else {
    parity = 1;
    xend = a2-1;
  }
  plot(x,y, reverse);
  incr2 = 4*dy - 2*dx;
  // slope is less than 1/2
  if (incr2 < 0)
  {
    c=2*dy;
    incr1 = 2*c;
    D = incr1 - dx;
    while (x != xend)
    {
      if (D<0)
      {
        //	draw(1);
        ++x;
        plot(x,y, reverse);
        ++x;
        plot(x,y, reverse);
        // end of draw(1);
        D=D + incr1;
      } else {
        if (D<c)
        {
          //	draw(2);
          ++x;
          plot(x,y, reverse);
          ++x;
          y+=step;
          plot(x,y, reverse);
          // end of draw(2);
        } else {
          //	draw(3);
          ++x;
          y+=step;
          plot(x,y, reverse);
          ++x;
          plot(x,y, reverse);
          // end of draw(3);
        }
        D = D + incr2;
      }
    }
    //slope is >= 1/2
  } else {
    c=2*(dy-dx);
    incr1 = 2*c;
    D = incr1 + dx;
    while (x != xend)
    {
      if (D>=0) {
        //	draw(4);
        ++x;
        y+=step;
        plot(x,y, reverse);
        ++x;
        y+=step;
        plot(x,y, reverse);
        // end of draw(4);
        D=D+incr1;
      } else {
        if (D<c)
        {
          //	draw(2);
          ++x;
          plot(x,y, reverse);
          ++x;
          y+=step;
          plot(x,y, reverse);
          // end of draw(2);
        } else {
          //	draw(3);
          ++x;
          y+=step;
          plot(x,y, reverse);
          ++x;
          plot(x,y, reverse);
          // end of draw(3);
        }
        D = D+incr2;
      }
    }
  }
  if (parity == 1)
  {
    plot(a2,b2, reverse);
  }
  
}

void wurokne_test()
{
  al_flip_display();
  bmp = al_get_backbuffer(display);
  al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE );
  for (int i = 300; i<=400; i++)
  {
    for (int j = 300; j<=400; j++)
    {
      wurokne(350, 350, i, j);
    }
  }
  al_unlock_bitmap(bmp);
  al_flip_display();
}

void przyrostowy_test()
{
  al_flip_display();
  bmp = al_get_backbuffer(display);
  al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE );
  for (int i = 100; i<=200; i++)
  {
    for (int j = 100; j<=200; j++)
    {
      przyrostowy(150, 150, i, j);
    }
  }
  al_unlock_bitmap(bmp);
  al_flip_display();  
}

// wysypuje się przy większych obszarach
void FloodFill_rec(int x, int y)
{
	if (x >= 0 && x <= SCREEN_W && y >= 0 && y <= SCREEN_H)
	{
		al_set_target_bitmap(al_get_backbuffer(display));
		al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x,y), &pix_r, &pix_g, &pix_b);
		if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
		{
			al_put_pixel(x,y, black);
      // w dół
      FloodFill_rec(x,y+1);
      // w górę
      FloodFill_rec(x, y-1);
      // w prawo
      FloodFill_rec(x+1, y); 
      // w lewo
      FloodFill_rec(x-1, y);
     
    }
  }
}


// nie wysypuje się nawet przy wielkich powierzchniach :)
void FloodFill_iter(int x, int y)
{
  struct queue Q;
  init_queue(&Q);
	if (!(x >= 0 && x <= SCREEN_W && y >= 0 && y <= SCREEN_H))
    return;
  al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x,y), &pix_r, &pix_g, &pix_b);
  if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
  {
    enqueue(&Q, x);
    enqueue(&Q, y);
    while (!queue_empty_p(&Q))
    {
      dequeue(&Q, &x);
      dequeue(&Q, &y);
      al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x,y), &pix_r, &pix_g, &pix_b);
      if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
      {
        al_put_pixel(x, y, black);
      }
      if (x-1>=0)
      {
        al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x-1,y), &pix_r, &pix_g, &pix_b);
        if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
        {
          al_put_pixel(x-1, y, black);
          enqueue(&Q, x-1);
          enqueue(&Q, y);
        }
      }
      if (x+1<=SCREEN_W)
      {
        al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x+1,y), &pix_r, &pix_g, &pix_b);
        if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
        {
          al_put_pixel(x+1, y, black);
          enqueue(&Q, x+1);
          enqueue(&Q, y);
        }
      }
      if (y-1>=0)
      {
        al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x,y-1), &pix_r, &pix_g, &pix_b);
        if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
        {
          al_put_pixel(x, y-1, black);
          enqueue(&Q, x);
          enqueue(&Q, y-1);
        }
      }
      if (y+1<=SCREEN_H)
      {
        al_unmap_rgb(al_get_pixel (al_get_backbuffer( display ), x,y+1), &pix_r, &pix_g, &pix_b);
        if ((int)pix_r != 0 && (int)pix_g != 0 && (int)pix_b != 0)
        {
          al_put_pixel(x, y+1, black);
          enqueue(&Q, x);
          enqueue(&Q, y+1);
        }
      }
    }
  }
}

void FloodFill_run (int x, int y)
{
  al_flip_display();
  bmp = al_get_backbuffer( display );
  al_lock_bitmap( bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE );
  al_set_target_bitmap(al_get_backbuffer(display));
  FloodFill_iter(x,y);
 // FloodFill_rec(x,y);
  al_unlock_bitmap( bmp );
  al_flip_display();
  
}

int main(int argc, char **argv)
{
	bool doexit = false;
	bool wurok = false;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *bouncer = NULL;
	ALLEGRO_BITMAP *bouncer2 = NULL;

	bool line_status = false;
	int start_x;
	int start_y;
	black = al_map_rgb(0, 0, 0);
	float bouncer_x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	float bouncer_y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
	
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
  
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
  
	
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize the mouse!\n");
		return -1;
	}
	
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}
	
	timer = al_create_timer(1.0 / 60);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
  
	
	bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
	bouncer2 = al_create_bitmap(BOUNCER_SIZE-2, BOUNCER_SIZE-2);
	
	al_clear_to_color(al_map_rgb(255, 255, 255));
	
	al_set_target_bitmap(bouncer);
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	
	al_set_target_bitmap(bouncer2);
	
	al_clear_to_color(al_map_rgb(255, 255, 255));
	
	al_set_target_bitmap(al_get_backbuffer(display));
	
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(bouncer);
		al_destroy_display(display);
		return -1;
	}
	
	al_register_event_source(event_queue, al_get_display_event_source(display));
	
	al_register_event_source(event_queue, al_get_mouse_event_source());

	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	
	
	al_draw_bitmap(bouncer, bouncer_x, bouncer_y, 0);
	al_draw_bitmap(bouncer2, bouncer_x+1, bouncer_y+1, 0);
	
	
	al_flip_display();
  
  printf("INSTRUCTION\n");
  printf("LPM - draw line\nPPM - flood fill\n");
  printf("ESC - Quit program\nW - switch to draw lines using Wu Rokne algorithm\n");
  printf("P - switch to draw lines using DDA algorithm\nT - test DDA algorithm (should draw a square)\n");
  printf("Y - test WuRokne algorithm (should draw a square)\nB - benchmark of DDA and WuRokne algorithm (clears screen!)\n");
  printf("C - clear screen\n");
  
	while(!doexit)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
    if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		} else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
		{
			if (ev.mouse.button == 2)
			{

				FloodFill_run(ev.mouse.x, ev.mouse.y);

			} else {
				if (line_status)
				{
          al_flip_display();
					bmp = al_get_backbuffer(display);
					al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE );
					if (wurok) {
						wurokne(start_x, start_y, ev.mouse.x, ev.mouse.y);
					} else {
					przyrostowy(start_x, start_y, ev.mouse.x, ev.mouse.y);
					}
					al_unlock_bitmap(bmp);
					al_flip_display();
					line_status = false;
				} else {
					start_x = ev.mouse.x;
					start_y = ev.mouse.y;
					line_status = true;
				}
			}
		} else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
				switch (ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					printf("Pressed ESC key - quitting program\n");
					doexit = true;
					break;	
				case ALLEGRO_KEY_W:
					printf("Pressed W key - switching to drawing lines using WuRokne algorithm\n");
					wurok = true;
					break;
				case ALLEGRO_KEY_P:
					printf("Pressed P key - pswitching to drawing lines using DDA algorithm\n");
					wurok = false;
					break;
        case ALLEGRO_KEY_T:
					printf("Pressed T key - DDA algorithm test\n");
					przyrostowy_test();
					break;
        case ALLEGRO_KEY_Y:
					printf("Pressed Y key - WuRokne algorithm test\n");
					wurokne_test();
					break;
        case ALLEGRO_KEY_B:
					printf("Pressed B key - performing DDA vs WuRokne benchmark\n");
					clear_screen();
          al_flip_display();
					bmp = al_get_backbuffer(display);
          al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE );
          int clo = clock();
          for (int w = 0; w<=1000000;w++)
          {
            przyrostowy(0, 0, 600, 600);
          } 
          int przyr_time = (clock() - clo);
          al_unlock_bitmap(bmp);
          clear_screen();
          al_flip_display();
					bmp = al_get_backbuffer(display);
          al_lock_bitmap(bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE );
          clo = clock();
          for (int w = 0; w<=1000000;w++)
          {
            wurokne(0, 0, 600, 600);
          } 
          int wurokne_time = (clock() - clo);
          al_unlock_bitmap(bmp);
          clear_screen();
          printf("Test completed, results for 100 000 lines:\n");
          printf("DDA: %.6f seconds\n", ((przyr_time/1000.0)/1000.0));
          printf("WuRokne: %.6f seconds\n", ((wurokne_time/1000.0)/1000.0));
          printf("\n");
					break;
				case ALLEGRO_KEY_C:
					printf("Pressed C key - clearing screen\n");
          clear_screen();
          line_status = false;
					break;
					
			}
		}
	}	
	
	al_destroy_bitmap(bouncer);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	
	return 0;
}

