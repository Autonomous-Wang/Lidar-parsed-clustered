#include "functions.h"
int main()
{
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses return in bytes*/
	int recvlen;			/* # bytes received */
	int fd;				/* our socket */
	unsigned char buf[BUFSIZE];	/* receive buffer */
	/* create a UDP socket */

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
	{
		perror("bind failed");
		return 0;
	}
    int i;
    int num = 77; // number of packets to be received
    int f = 1; // to store the number of frame
    /* now loop, receiving data and printing what we received */
  
	for (i = 1; i < num + 1; i++) // constantly receing packets
    //for (; ; ) 
	{
		printf("waiting on port %d\n", SERVICE_PORT);
		recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		printf("received %d bytes\n", recvlen);
       
		FILE *fw = fopen("file.txt","wb"); // everytime creat a new file and overwrite
        fwrite(&buf, 1, sizeof(buf), fw);        	
        fclose(fw);

        // creat node to store data
        struct node *newNode = (struct node*)malloc(sizeof(struct node));
        if(newNode == NULL)
        {
            fprintf(stderr, "can not assign memory space\n");
            exit(-1);
        }
        FILE *fr = fopen("file.txt", "rb");
        newNode->next = NULL;
        newNode->id = i;
        parse(i, fr); // i is used to name the file
        newNode->stmp = timestmp(fr);
        fclose(fr);
        if(head == NULL)
        {
        	head = newNode;   /* Head is the first node = same node */
    	    tail = newNode;  
        }
        // and then combine the node content, each node contain one frame data
        else
        {
    	    tail->next = newNode;
    	    tail = newNode; 
        }
        // combine node according to the time stamp within the 0.1 second
        if (newNode->stmp - head->stmp < 0.1) 
        {
        	// combine all the data packets
        	appendfile(newNode->id, f);
        }
        else
        {
            /*char filename1[20];
            sprintf(filename1, "#%dframe.txt", f);
            fframe = fopen("filename1","r");
            newFrame->frame = fframe;*/
            struct frame *newFrame = (struct frame*)malloc(sizeof(struct frame));
            newFrame->id = f;
        	newFrame->stmp = head->stmp;
        	newFrame->next = NULL;
        	head = move(head, i);
            
            // employ DBSCAN to the frame data and first should specify the size of each frame

            FEATURES = 3; // 3D data
            DATASET_SIZE = rows(f); // the datasize in each frame
            EPSILON = 0.25;
            MIN_POINTS = 30;


            char framename[20];
            sprintf(framename, "#%dframe.txt", f);
            loadData(framename);
            dbScan();
            printData(f);
            f = f + 1;

        	if(head_f == NULL)
           {
        	    head_f = newFrame;   
    	        tail_f = newFrame;  
            }
        // and then combine the node content, each node contain one frame data
            else
            {
    	        tail_f->next = newFrame;
    	        tail_f = newFrame; 
            }
        }
    }
    // print all the data in the linked list for checking
    checklist(head);
}


