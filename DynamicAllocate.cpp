#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef enum{
	FREE,ALLO
}MemoryState; 

typedef struct MemoryBlock{
	struct MemoryBlock *pre;
	struct MemoryBlock *next;
	unsigned long long size;
	unsigned long long addr;
	unsigned long long pid;
	MemoryState state;
}MemoryBlock;

typedef struct MemoryAllo{
	unsigned long long size;
	unsigned long long pid;
}MemoryAllo;

MemoryBlock *g_freememory = NULL;
MemoryBlock *g_allomenory = NULL;
const int g_minsize = 1;

void display();
bool MemoryAllocate_Firstfit(MemoryAllo &u);
bool MemoryAllocate_Bestfit(MemoryAllo &u);
MemoryBlock* findAllo(MemoryAllo &u);
void deleteNode(MemoryBlock* node,MemoryBlock* &list);
void addFirst(MemoryBlock* node,MemoryBlock* &list);
void addPre(MemoryBlock* node_pre,MemoryBlock* node);
void addNext(MemoryBlock* node_next,MemoryBlock *node);
bool MemoryRelease(MemoryAllo &un);

bool MemoryAllocate_Firstfit(MemoryAllo &u)
{
	MemoryBlock *m;

	m = g_freememory;
	while(m)
	{
		long long left = m->size - u.size;
		if(left < 0 || (left >0 && left < g_minsize))		
		{
			m = m->next;
		}
		else
		{
			MemoryBlock *temp = NULL;
			if(left == 0)
			{
				m->pid = u.pid;
				m->state = ALLO;
				if((temp = m->pre)) temp->next = m->next;
				if((temp = m->next)) temp->pre = m->pre;
				m->pre = NULL;
				m->next = g_allomenory;
				if(g_allomenory) g_allomenory->pre = m;
				g_allomenory = m;
			}
			else
			{
				MemoryBlock *alloblock = (MemoryBlock*)malloc(sizeof(MemoryBlock));
				alloblock->size = u.size;
				alloblock->addr = m->addr;
				alloblock->state = ALLO;
				alloblock->pid = u.pid;
				alloblock->pre = NULL;
				alloblock->next = g_allomenory;
				if(g_allomenory) g_allomenory->pre = alloblock;
				g_allomenory = alloblock;
				m->size = left;
				m->addr+=u.size;
			}
			return true;
		}
	}
	return false;
}

bool MemoryAllocate_Bestfit(MemoryAllo &u)
{
	MemoryBlock *m,*minBlock,*temp;
	
	m = g_freememory;
	while(m)
	{
		long long left = m->size - u.size;
		if(left < 0 || (left > 0 && left < g_minsize))
		{
			m = m->next;
		}
		else
		{
			if(m->size > u.size)
			{
				if(minBlock == NULL || minBlock->size > m->size)
					minBlock = m;
				else
					m = m->next;
			}
			else
			{
				break;
			}
		}
	}
	if(minBlock)
	{
		long long left = minBlock->size - u.size;
		if(left == 0)
		{
			m->pid = u.pid;
			m->state = ALLO;
			if((temp = m->pre)) temp->next = m->next;
			if((temp = m->next)) temp->pre = m->pre;
			m->pre = NULL;
			m->next = g_allomenory;
			if(g_allomenory) g_allomenory->pre = m;
			g_allomenory = m;
		}
		else
		{
			MemoryBlock *alloblock = (MemoryBlock*)malloc(sizeof(MemoryBlock));
			alloblock->size = u.size;
			alloblock->addr = minBlock->addr;
			alloblock->state = ALLO;
			alloblock->pid = u.pid;
			alloblock->pre = NULL;
			alloblock->next = g_allomenory;
			if(g_allomenory) g_allomenory->pre = alloblock;
			g_allomenory = alloblock;
			minBlock->size = left;			
			minBlock->addr+=u.size;
		}
		return true;
	}
	return false;
}

MemoryBlock* findAllo(MemoryAllo &u)
{
	MemoryBlock *m = g_allomenory;

	while(m)
	{
		if(m->size == u.size && m->pid == u.pid)
			return m;
		m = m->next;
	}
	return NULL;
}

void deleteNode(MemoryBlock* node,MemoryBlock* &list)
{
	if(node == NULL || list == NULL)
		return;
	if(node == list)
	{
		list = list->next;
		if(list) list->pre = NULL;
	}
	else
	{
		MemoryBlock *temp;
		node->pre->next = node->next;
		if(node->next) node->next->pre = node->pre;
	}
	node->next = node->pre = NULL;
}

void addFirst(MemoryBlock* node,MemoryBlock* &list)
{
	if(node == NULL)
		return;

	if(list == NULL)
		node->pre = node->next = NULL;
	else
		addPre(node,list);
	list = node;
}


void addPre(MemoryBlock* node_pre,MemoryBlock* node)
{
	if(node == NULL || node_pre == NULL)
		return;
	node_pre->pre = node->pre;
	node_pre->next = node;
	if(node->pre) node->pre->next = node_pre;
	node->pre = node_pre;
}

void addNext(MemoryBlock* node_next,MemoryBlock *node)
{
	if(node == NULL || node_next == NULL)
		return;
	node_next->pre = node;
	node_next->next = node->next;
	node->next = node_next;
	if(node_next->next) node_next->next->pre = node_next;
}

bool MemoryRelease(MemoryAllo &un)
{
    MemoryBlock *release, *temp;

    release = findAllo(un);
    if (release == NULL)
        return false;

    deleteNode(release, g_allomenory);
    release->state = FREE;

    MemoryBlock *m, *pre;
    m = g_freememory;
    pre = NULL;
    while (m)
    {
        if (release->addr > m->addr)
        {
            pre = m;
            m = m->next;
        }
        else
            break;
    }
    if (pre)
    {
        if (pre->addr + pre->size == release->addr)
        {
            pre->size += release->size;
            free(release);
            deleteNode(pre, g_freememory);
            release = pre;
        }
    }
    else
    {
        if (m)
        {
            if (release->addr + realease->size == m->addr)
            {
                m->size += release->size;
                m->addr = releaase->addr;
                free(release);
            }
            else
            {
                addPre(release, m);
            }
        }
        else
        {
            addFirst(release, g_freememory);
        }
    }
    return true;
}

void initMemory(unsigned long long size)
{
	MemoryBlock *block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
	memset(block,0,sizeof(MemoryBlock));
	block->size = size;
	block->state = FREE;
	g_freememory = block;
	g_allomenory = NULL;
}

void display()
{
	printf("PID\tAddress\tSize\tState\n");
	for(MemoryBlock *m = g_freememory; m != NULL; m = m->next)
	{
		printf("%lld\t%lld\t%lld\t%s\n",m->pid,m->addr,m->size,"Free");
		fflush(stdout);
	}
	for(MemoryBlock *m = g_allomenory; m != NULL; m = m->next)
	{
		printf("%lld\t%lld\t%lld\t%s\n",m->pid,m->addr,m->size,"Allocate");
		fflush(stdout);
	}
}

int main(int argc, char const *argv[])
{
	MemoryAllo block;
	unsigned long long size[] = {130,60,100,60,200,100,130,140,60,50,60};
    unsigned long long pid[] = {1,2,3,2,4,3,1,5,6,7,8};
	MemoryState state[] = {ALLO,ALLO,ALLO,FREE,ALLO,FREE,FREE,ALLO,ALLO,ALLO,ALLO};
	int op = 1;
	initMemory(640);
	printf("start...\n");
	printf("please select 1/2 to choose Firstfit or Bestfit to allocate memory: ");
	scanf("%d",&op);
	
	for(unsigned long i = 0; i < sizeof(size)/sizeof(unsigned long long); i++)
	{
		block.size = size[i];
		block.pid = pid[i];
		if(state[i] == ALLO)
			op == 1 ? MemoryAllocate_Firstfit(block):MemoryAllocate_Bestfit(block);
		else
			MemoryRelease(block);
		display();
	}

	return 0;
}

