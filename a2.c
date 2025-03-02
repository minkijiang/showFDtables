#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ALLPID -1
#define MAXLENGTH 256

typedef struct FD {
	int fd;
	char file[MAXLENGTH];
	int inode;
	struct FD* next;
} FD;

typedef struct PROCESS {
	int numFd;
	int pid;
	FD* FDarr;
} PROCESS;


FD* createFD(int fd, char file[MAXLENGTH], int inode) {
	FD* newFD = malloc(sizeof(FD));
	newFD->fd = fd;
	strcpy(newFD->file,file);
	newFD->inode = inode;
	newFD->next = NULL;

	return newFD;
}

PROCESS* createPROCESS(int pid) {
	PROCESS* newProcess = malloc(sizeof(PROCESS));
	newProcess->pid = pid;
	newProcess->numFd = 0;
	newProcess->FDarr = NULL;

	return newProcess;
}

void freeFD(FD* fd) {
	for (FD* i = fd; i != NULL; i = i->next) {
		free(i);
	}
}

void freePROCESS(PROCESS* process) {
	free(process->FDarr);
	free(process);
}

void displayProcessFD(PROCESS* process) {
	FD* fd = process->FDarr;

	printf("\n\n");
	printf("PID			FD\n");
	printf("==============\n");
	for (FD* i = fd; i != NULL; i = i->next) {
		printf("%d   %d\n", process->pid, i->fd);
	}
	printf("==============\n");
}

void displaySystemWide(PROCESS* process) {
	FD* fd = process->FDarr;

	printf("\n\n");
	printf("PID			Filename\n");
	printf("====================\n");
	for (FD* i = fd; i != NULL; i = i->next) {
		printf("%d   %s\n", process->pid, i->file);
	}
	printf("====================\n");
}

void displayVnode(PROCESS* process) {
	FD* fd = process->FDarr;

	printf("\n\n");
	printf("PID			Inode\n");
	printf("=================\n");
	for (FD* i = fd; i != NULL; i = i->next) {
		printf("%d   %d\n", process->pid, i->inode);
	}
	printf("=================\n");
}

void displayComposite(PROCESS* process) {
	FD* fd = process->FDarr;

	printf("\n\n");
	printf("PID			FD			Filename			Inode\n");
	printf("=============================================\n");
	for (FD* i = fd; i != NULL; i = i->next) {
		printf("%d   %d   %s   %d\n", process->pid, i->fd, i->file, i->inode);
	}
	printf("=============================================\n");
}

void displaySummary(PROCESS** processArr, int numProcess) {
	printf("\n\n");
	printf("Summary Table\n");
	printf("=============\n");

	for (int i = 0; i< numProcess; i++) {
		int pid = processArr[i]->pid;
		int numFd = processArr[i]->numFd;
		printf("%d : %d FD\n", pid, numFd);
	}
	printf("=============\n");
}

int getAllProcesses(PROCESS** processArr) {
	int processCount = 0;

	//PROCESS* process = createPROCESS(pid);
	//getFD(process);


	return processCount;
}

void getFD(PROCESS* process) {

}

void display(int pid, int isProcessFD, int isSystemWide, int isVnode, int isComposite, int isSummary) {
	PROCESS* process = createPROCESS(pid);
	getFD(process);

	if (isProcessFD == 1) {displayProcessFD(process);}
	else if (isSystemWide == 1) {displaySystemWide(process);}
	else if (isVnode == 1) {displayVnode(process);}
	else if (isComposite == 1) {displayComposite(process);}
	else if (isSummary == 1) {
		PROCESS** allProcesses;
		int processCount = getAllProcesses(allProcesses);
		displaySummary(allProcesses, processCount);
	}
}

void displayAll(int isProcessFD, int isSystemWide, int isVnode, int isComposite, int isSummary) {

	PROCESS** allProcesses;
	int processCount = getAllProcesses(allProcesses);

	if (isSummary == 1) {
		displaySummary(allProcesses, processCount);
	}
	for (int i = 0; i < processCount; i++ ) {
		PROCESS* process = *(allProcesses+i);
		if (isProcessFD == 1) {displayProcessFD(process);}
		else if (isSystemWide == 1) {displaySystemWide(process);}
		else if (isVnode == 1) {displayVnode(process);}
		else if (isComposite == 1) {displayComposite(process);}
	}
	


}



int test(int argc, char** argv) {
	int isProcessFD = 0;
	int isSystemWide = 0;
	int isVnode = 0;
	int isComposite = 0;
	int isSummary = 0;

	int pid = ALLPID;

	if (argc == 1) {
		isComposite = 1;
		pid = ALLPID;
	}
	else if (argc == 2 && strtol(argv[1], NULL, 10) > 0) {
		pid = strtol(argv[1], NULL, 10);
		isProcessFD = 1;
	    isSystemWide = 1;
	    isVnode = 1;
	    isComposite = 1;
	}
	else {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "--per-process") == 0 ) {
				isProcessFD = 1;
			}
			else if (strcmp(argv[i], "--systemWide") == 0 ) {
				isSystemWide = 1;
			}
			else if (strcmp(argv[i], "--Vnodes") == 0 ) {
				isVnode = 1;
			}
			else if (strcmp(argv[i], "--composite") == 0 ) {
				isComposite = 1;
			}
			else if (strcmp(argv[i], "--summary") == 0 ) {
				isSummary = 1;
			}
			else if (i == 0 && strtol(argv[i], NULL, 10) > 0) {
				pid = strtol(argv[i], NULL, 10);
			}
			else {
				fprintf(stdout, "Error: invalid arguments");
				exit(1);
			}
		}
	}

	if (pid == ALLPID) {
		displayAll(isProcessFD, isSystemWide, isVnode, isComposite, isSummary);
	}
	else {
		display(pid, isProcessFD, isSystemWide, isVnode, isComposite, isSummary);
	}
	


	return 0;
}

int main() {

	return 0;
}