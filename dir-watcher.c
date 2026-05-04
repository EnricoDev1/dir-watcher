#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#define EV_SIZE (sizeof(struct inotify_event))

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <watch-dir>\n", argv[0]);
    return 1;
  }
    
  int ifd = inotify_init1(0);
  if (ifd == -1) {
    perror("inotify_init");
    return 1;
  }
  #define IN_ALL (IN_ACCESS | IN_CREATE | IN_DELETE | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_DELETE | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO | IN_OPEN)
  
  int wfd = inotify_add_watch(ifd, argv[1], IN_ALL);
  if (wfd == -1) {
    perror("inotify_add_watch");
    return 1;
  }
  
  char *buf = malloc(0x1000 * sizeof(char));

  while(1) {
    int nr = read(ifd, buf, 0x1000);
    if (nr < 0) {
      perror("read");
      return 1;
    }
    
    for(int i = 0; i < nr;) {
      struct inotify_event *ev = (struct inotify_event *) &buf[i];
      if (ev->len == 0) {
        i += EV_SIZE;
        continue;
      }
      
      if (ev->mask & IN_CREATE) printf("Created");
      else if (ev->mask & IN_MODIFY) printf("Modified");      
      else if (ev->mask & IN_ACCESS) printf("Accessed");
      else if (ev->mask & IN_ATTRIB) printf("Metadata changed");
      else if (ev->mask & IN_CLOSE_WRITE) printf("Closed from writing");
      else if (ev->mask & IN_CLOSE_NOWRITE) printf("Closed from no-writing");
      else if (ev->mask & IN_DELETE) printf("Deleted");
      else if (ev->mask & IN_MOVE_SELF) printf("Moved watch");
      else if (ev->mask & IN_MOVED_FROM) printf("Moved from");
      else if (ev->mask & IN_MOVED_TO) printf("Moved to");
      else if (ev->mask & IN_OPEN) printf("Opened");
      
      printf(" %s\n", ev->name);
            
      i += EV_SIZE + ev->len;
    }
  }

  free(buf);
  inotify_rm_watch(ifd, wfd);
  close(ifd);
  
  return 0;
}
