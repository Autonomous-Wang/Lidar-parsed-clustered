#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>

#define BUFSIZE 1248
#define SERVICE_PORT 2368
#define PI 3.1415926

// DBSCAN parameters
double **data;
int *clusters;
int *visited;
int *neigh_points;

int FEATURES;
int DATASET_SIZE; // should change accourding to actual frame
float EPSILON;
int MIN_POINTS;

// this function to combine file into one whose time stamp within 0.1 seconds
void appendfile(int num, int frame_num) // append file f2 at the end of frame file
{
    char filename[20], framename[20];  
    sprintf(filename, "#%dpacket.txt", num); 
    sprintf(framename, "#%dframe.txt", frame_num);

    int set;
    FILE *fr = fopen(framename, "a");
    FILE *f = fopen(filename, "r");
    char buffer[50]; // read each line in the frame as string
    if(f == NULL) 
    {
        perror("Error opening file.");
    }
    else 
    {
        while(fgets(buffer, sizeof(buffer), f)) 
        {
        fprintf(fr, "%s", buffer);
        }
        //fprintf(fr, "\n");
    }
    fclose(f);
    fclose(fr);
}

void delete(int num)
{
    int set;
    char filepacket[20];
    sprintf(filepacket, "#%dpacket.txt", num); 
    set = remove(filepacket); // delete packet data for convenience
}

// check all the timestamp in the linked list and frame at given position

// for calculation in parse packet file
int hex(int num1, int num2)
{
    int y;
    y = num1*16*16+num2;
    return y;
}

// for finding the head pointer for new frame

// parsing the packet received
void parse(int num, FILE *ptr_myfile) // num is used for naming the parsed data
{
    int counter;
    unsigned char buff[1206];
    unsigned int x[1206];
    if (!ptr_myfile)
    {
        printf("Unable to open file!");
    }
    fread(buff, 1, sizeof(buff), ptr_myfile);
    for (counter = 0; counter < sizeof(buff); counter++)
    {
        x[counter] = buff[counter];
    }
    fclose(ptr_myfile);
    
    double omega[32] = {-15,1,-13,-3,-11,5,-9,7,-7,9,-5,11,-3,13,-1,15,-15,1,-13,-3,-11,5,-9,7,-7,9,-5,11,-3,13,-1,15};
    // tansfer into pi
    int i1;
    for (i1=0; i1<32; i1++)
    {
        omega[i1]=omega[i1]*PI/180;
    }
    double Azimuth[2][12];
    double Distance_channel[32];
    double X[384];
    double Y[384];
    double Z[384];
    unsigned int Inten[384];
    double Datablock[100];
    double Azimuth1, Azimuth2, Distance1, Distance2;
    double diff;
    int j;
    int k;
    int m;
    for (j=0; j<12; j++)
    {
        for (k=0; k<100; k++)
        {
            Datablock[k] = x[100*j+k];
        }
        Azimuth1 = Datablock[3];
        Azimuth2 = Datablock[2];
        Azimuth[0][j] = hex(Azimuth1, Azimuth2)/100*PI/180;
    }
    diff = Azimuth[0][11] - Azimuth[0][0];
    if (diff < 0)
    {
        diff = (diff + 2*PI)/11;
    }
    else
    {
        diff = diff/11.0;
    }
    for (j=0; j<11; j++)
    {
        if (Azimuth[0][j] > Azimuth[0][j+1])
       {
        Azimuth[1][j] = (Azimuth[0][j] + Azimuth[0][j+1]+2*PI)/2;
       }
       else
       {
        Azimuth[1][j] = (Azimuth[0][j] + Azimuth[0][j+1])/2.0;
       }
       if (Azimuth[1][j] > 2*PI)
       {
        Azimuth[1][j] = Azimuth[1][j] -2*PI;
       }
    }    

    Azimuth[1][11] = (Azimuth[0][0] + Azimuth[0][11] + diff)/2;
    if (Azimuth[1][11] > 2*PI)
    {
        Azimuth[1][11]= Azimuth[1][11]-2*PI;
    }
        // Distance calculation
        for (j=0; j<12; j++)
        {
            for (k=0; k<100; k++)
           {
                Datablock[k] = x[100*j+k];
            }
            for (m=0; m<32; m++)
           {   
            // distance calculation
            Distance1 = Datablock[m*3+5];
            Distance2 = Datablock[m*3+4];
            Distance_channel[m] = hex(Distance1, Distance2)*2/1000.0;

            // store intensity value
            Inten[32*j+m] = Datablock[m*3+6];
            
            // from sphere to xyz coordinate
                if (m < 16)
                {
                    X[32*j+m]=Distance_channel[m]*cos(omega[m])*sin(Azimuth[0][j]);
                    Y[32*j+m]=Distance_channel[m]*cos(omega[m])*cos(Azimuth[0][j]);
                    Z[32*j+m]=Distance_channel[m]*sin(omega[m]);
                }
                else
                {
                    X[32*j+m]=Distance_channel[m]*cos(omega[m])*sin(Azimuth[1][j]);
                    Y[32*j+m]=Distance_channel[m]*cos(omega[m])*cos(Azimuth[1][j]);
                    Z[32*j+m]=Distance_channel[m]*sin(omega[m]);
                }
            }
        }
    // change X, Y, Z into matrix and return it to main function
    // use the defined unrolled linked list to store X, Y and Z coordinate
    int n;
    // write processed data to file and plot and this is not necessary needed.
    FILE *temp;
    char filename[20]; 
    sprintf(filename, "#%dpacket.txt", num); 
    temp = fopen(filename, "w+");
    // FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    for (n = 0; n < 384; n++)
    {
        // range can also be specified in the following statement, to filter out unecessary points
        if (!(X[n]==0 && Y[n]==0 && Z[n] == 0)) // remove all zero values and can also add filter
        {
            if (X[n] < 20 && X[n] > -20 && Y[n] < 20 && Y[n] > -20 && Z[n] > -0.8 && Z[n] < 1)
            {
                fprintf(temp, "%lf %lf %lf\n", X[n], Y[n], Z[n]);
            }
        }
    }
    //fprintf(gnuplotPipe, "%s \n", "splot 'XYZ.txt' with points pointtype 7 pointsize 0.8");
    //fclose(gnuplotPipe);
    fclose(temp);
 }

// plot the data in one parsed packet
void showframe(int num)
{
    char filename[20];
    sprintf(filename, "#%dframe.txt", num); 
    FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");
    fprintf(gnuplotPipe, "%s \n", "splot 'filename' with points pointtype 7 pointsize 0.8");
    fclose(gnuplotPipe);
}

int rows(int num)
{
    // to decide the number of lines in one frame
    char filename[20];
    sprintf(filename, "#%dframe.txt", num);
    FILE *f = fopen(filename, "r");
    char buffer[384*4];
    int count = 0;
    if(f == NULL) 
    {
        perror("Error opening file.");
    }
    else 
    {
        while(fgets(buffer, sizeof(buffer), f)) 
        {
            count = count + 1;
        }
    }
    fclose(f);
    return count;
} 

double timestmp(FILE *ptr_myfile)
{
    // getting the time stamp byte
    int counter, i;
    unsigned char buff[1206];
    unsigned int x[1206];
    unsigned int time[4];
    double stamp; // in microseconds
    if (!ptr_myfile)
    {
        printf("Unable to open file!");
        return 1;
    }
    fread(buff, 1, 1206, ptr_myfile);
    for (counter = 0; counter < 1206; counter++)
    {
        x[counter] = buff[counter];
    }
    fclose(ptr_myfile);
    // timestamp is stored as four byte values
    for (i = 0; i < 4; i++)
    {
        time[i] = x[i + 1200]; // store in reverse order to process it easier
    }
    // transfer the byte value into integer as micro-sencond
    stamp = (time[3]*256*256*256 + time[2]*256*256 + time[1]*256 + time[0]*1)/1000000.0; 
    return stamp;
}

// dbscan implementation
void loadData(char file_name[])
{
    data = (double**)calloc(DATASET_SIZE, sizeof(double*));
    
    visited = (int*)calloc(DATASET_SIZE, sizeof(int));
    
    neigh_points = (int*)calloc(DATASET_SIZE*DATASET_SIZE, sizeof(int));
    
    clusters = (int*)calloc(DATASET_SIZE, sizeof(int));
    
    int i, j;
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        data[i] = (double*)calloc(FEATURES, sizeof(double));
    }
    
    FILE *fp;
    fp = fopen(file_name, "r");
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        for(j = 0; j < FEATURES; j++)
        {
            fscanf(fp, "%lf", &data[i][j]);
        }
    }
    
    fclose(fp);
}

int regionQuery(int start, int index)
{
    int i, j, count = 0;
    double distance, temp;
    for(i = 0; i < DATASET_SIZE; i++)
    {
        if(i != index)
        {
            distance = 0;
            for(j = 0; j < FEATURES; j++)
            {
                temp = data[i][j] - data[index][j];
            
                distance += temp * temp;
            }
            if(distance <= EPSILON)
            {
                neigh_points[start+count] = i; // the position index of points in the epsilon region of the data set
                count++; // stands for the number of points in the epsilon region
            }
        }
    }
    return count;
}

void printData(int f)
{
    int i, j;
    char framename[20];
    FILE *fp;
    sprintf(framename, "#%dDbframe.txt", f);
    fp = fopen(framename, "w"); // need to change to match the frame number
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        //fprintf(fp, "%d\n", clusters[i]);
        if (clusters[i]!=0) // rule out the noise point
        {
            fprintf(fp, "%d\n", clusters[i]);
        }
    }
}

void expandCluster(int cluster_no, int num_npoints, int index)
{
    clusters[index] = cluster_no;
    
    int i, count = 0;
    
    for(i = 0; i < num_npoints; i++)
    {
        if(!visited[neigh_points[i]])
        {
            visited[neigh_points[i]] = 1;
            
            count = regionQuery(num_npoints, neigh_points[i]); // searching surrounding points
            
            if(count >= MIN_POINTS)
            {
                num_npoints += count;
            }
        }
        
        if(!clusters[neigh_points[i]])
        {
            clusters[neigh_points[i]] = cluster_no;
        }
    }
}

void dbScan()
{
    int next_cluster = 1, i, j, num_npoints;
    
    for(i = 0; i < DATASET_SIZE; i++)
    {
        if(!visited[i])
        {
            visited[i] = 1;
            
            num_npoints = regionQuery(0, i);
            
            if(num_npoints > MIN_POINTS)
            {
                expandCluster(next_cluster, num_npoints, i); // num_npoints: number of points in epsilon region
                
                next_cluster++;
            }
        }
    }
}   

int main()
{
    struct sockaddr_in myaddr;  /* our address */
    struct sockaddr_in remaddr; /* remote address */
    
    socklen_t addrlen = sizeof(remaddr);        /* length of addresses return in bytes*/
    int recvlen;            /* # bytes received */
    int fd;             /* our socket */
    unsigned char buf[BUFSIZE]; /* receive buffer */
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
    int num = 200; // number of packets to be received
    int f = 1; // to store the number of frame
    /* now loop, receiving data and printing what we received */
  
    //for (i = 1; i < num+1; i++) // constantly receing packets
    for (i = 1; i < num+1; i++) 
    {
        printf("waiting on port %d\n", SERVICE_PORT);
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        printf("received %d bytes\n", recvlen);
       
        FILE *fw = fopen("file.txt","wb"); // everytime creat a new file and overwrite
        fwrite(&buf, 1, sizeof(buf), fw);           
        fclose(fw);

        // creat node to store data
        FILE *fr = fopen("file.txt", "rb");
        int head_stmp, stmp;
        parse(i, fr); // i is used to name the file
        stmp = timestmp(fr);
        if (i == 1)
        {
            head_stmp = stmp;
        }
        fclose(fr);
        // combine node according to the time stamp within the 0.1 second
        if (stmp - head_stmp < 0.1) 
        {
            // combine all the data packets
            appendfile(i, f);
        }
        if (stmp - head_stmp >= 0.1)
        {
            appendfile(i, f + 1); // append the file to a new frame
            head_stmp = stmp;

            // employ DBSCAN to the frame data and first should specify the size of each frame
            FEATURES = 3; // 3D data
            DATASET_SIZE = rows(f); // the datasize in each frame
            EPSILON = 0.5 * 0.5;
            MIN_POINTS = 30;


            char framename[20];
            sprintf(framename, "#%dframe.txt", f);
            loadData(framename);
            dbScan();
            printData(f);
            free(data);
            free(visited);
            free(neigh_points);
            free(clusters);

            f = f + 1;
        }
        delete(i); // delete file which has already been appended at end of frame
    }
}



















