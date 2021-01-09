#include <iostream>
#include "thread.h"
#include "tcpserver.h"
#include "myserver.h"

using namespace std;

int main1()
{
    Thread *t1 = new Thread;
      Thread *t2 = new Thread;

      t1->start();
      t2->start();


      while(1)
      {
          printf("111111\n");
          sleep(1);
      }

    cout << "Hello World!" << endl;
    return 0;
}

int main2()
{
    TcpServer t;
    t.listen(8888);

    t.start();



    cout << "Hello World!" << endl;
    return 0;
}

int main()
{
    Myserver s;
    s.listen(8888);

    s.start();



    cout << "Hello World!" << endl;
    return 0;
}

