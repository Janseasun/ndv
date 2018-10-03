int send_all(int sockfd, char *buffer, int len);
int recv_all(int sockfd, char *buffer, int len);

int recv_line(int sockfd, char *buffer, int len);

int send_ball(int sockfd, char *buffer, int len);
int recv_ball(int sockfd, char *buffer, int len);
int set_jack_size(int, int);
void clean(int);
void hash(int s);
int init_jack();
