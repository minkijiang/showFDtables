#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ALLPID -1
#define MAXLENGTH 256
#define NOTHRESHOLD 0
#define NOTHING -2
#define END -3

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

typedef struct DISPLAYINFO {
	int isProcessFD;
	int isSystemWide;
	int isVnode;
	int isComposite;
	int isSummary;
	int outputTXT;
	int outputBIN;

	int pid;
	int threshold;
} DISPLAYINFO;


int validProcess(int pid) {
	if (pid == ALLPID) {
		return 1;
	}
	return 0;
}


FD* createFD(int fd, char file[MAXLENGTH], int inode) {
	FD* newFD = malloc(sizeof(FD));
	if (newFD == NULL) {
		fprintf(stderr, "Error: failed to malloc");
		exit(1);
	}

	newFD->fd = fd;
	strcpy(newFD->file,file);
	newFD->inode = inode;
	newFD->next = NULL;

	return newFD;
}

PROCESS* createPROCESS(int pid) {
	PROCESS* newProcess = malloc(sizeof(PROCESS));
	if (newProcess == NULL) {
		fprintf(stderr, "Error: failed to malloc");
		exit(1);
	}

	newProcess->pid = pid;
	newProcess->numFd = 0;
	newProcess->FDarr = NULL;

	return newProcess;
}

DISPLAYINFO* createDISPLAYINFO() {
	DISPLAYINFO* newDisplayInfo = malloc(sizeof(DISPLAYINFO));

	newDisplayInfo->isProcessFD = 0;
	newDisplayInfo->isSystemWide = 0;
	newDisplayInfo->isVnode = 0;
	newDisplayInfo->isComposite = 0;
	newDisplayInfo->isSummary = 0;
	newDisplayInfo->outputTXT = 0;
	newDisplayInfo->outputBIN = 0;

	newDisplayInfo->pid = ALLPID;
	newDisplayInfo->threshold = NOTHRESHOLD;

	return newDisplayInfo;
}

void freeFD(FD* fd) {
	for (FD* i = fd; i != NULL; i = i->next) {
		free(i);
	}
}

void freeAllPROCESS(PROCESS** processes, int processCount) {
	for (int i = 0; i < processCount; i++) {
		free(processes[i]->FDarr);
		free(processes[i]);
	}
	
}

void freeDISPLAYINFO(DISPLAYINFO* displayInfo) {
	free(displayInfo);
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

void writeCompositeTXT(PROCESS* process) {
	FILE* file = fopen("compositeTable.txt", "w");
	if (file == NULL) {
		fprintf(stderr, "Error: could not write to compositeTable.txt");
		exit(1);
	}

	FD* fd = process->FDarr;

	fprintf(file, "\n\n");
	fprintf(file, "PID			FD			Filename			Inode\n");
	fprintf(file, "==================================================\n");
	for (FD* i = fd; i != NULL; i = i->next) {
		fprintf(file, "%d   %d   %s   %d\n", process->pid, i->fd, i->file, i->inode);
	}
	fprintf(file, "==================================================\n");

	int isClosed = fclose(file);
	if (isClosed != 0) {
		fprintf(stderr, "Error: could not close compositeTable.txt");
		exit(1);
	}
}

void writeCompositeBIN(PROCESS* process) {
	FILE* file = fopen("compositeTable.bin", "wb");

	if (file == NULL) {
		fprintf(stderr, "Error: could not write to compositeTable.bin");
		exit(1);
	}

	FD* fd = process->FDarr;

	for (FD* i = fd; i != NULL; i = i->next) {
		fwrite(&process->pid, sizeof(int), 1, file);
		fwrite(&i->fd, sizeof(int), 1, file);
		fwrite(i->file, sizeof(char), MAXLENGTH, file);
		fwrite(&i->inode, sizeof(int), 1, file);
	}
	int end = END;
	fwrite(&end, sizeof(int), 1, file);

	int isClosed = fclose(file);
	if (isClosed != 0) {
		fprintf(stderr, "Error: could not close compositeTable.bin");
		exit(1);
	}
}

void displaySummary(PROCESS** processes, int numProcess) {
	printf("\n\n");
	printf("Summary Table\n");
	printf("=============\n");

	for (int i = 0; i< numProcess; i++) {
		int pid = processes[i]->pid;
		int numFd = processes[i]->numFd;
		printf("%d : %d FD\n", pid, numFd);
	}
	printf("=============\n");
}

void displayOffending(PROCESS** processes, int processNumber, int threshold) {
	printf("\n\n");
	printf("Offending Processes -- Threshold : %d\n", threshold);
	for (int i = 0; i < processNumber; i++) {
		int pid = processes[i]->pid;
		int numFd = processes[i]->numFd;
		if (numFd >= threshold) {
			printf("%d : %d FD\n", pid, numFd);
		}
		
	}
}

int getAllProcesses(PROCESS** processArr) {
	int processCount = 0;

	//PROCESS* process = getProcess(pid);


	return processCount;
}

PROCESS* getProcess(int pid) {
	PROCESS* process = createPROCESS(pid);
	return process;

}

void display(DISPLAYINFO* displayInfo) {

	PROCESS** allProcesses;
	int processCount = getAllProcesses(allProcesses);

	if (displayInfo->pid == ALLPID) {
		if (displayInfo->isSummary == 1) {displaySummary(allProcesses, processCount);}
		if (displayInfo->threshold != NOTHRESHOLD) {displayOffending(allProcesses, processCount,displayInfo->threshold);}

		for (int i = 0; i < processCount; i++ ) {
			PROCESS* process = *(allProcesses+i);
			if (displayInfo->isProcessFD == 1) {displayProcessFD(process);}
			if (displayInfo->isSystemWide == 1) {displaySystemWide(process);}
			if (displayInfo->isVnode == 1) {displayVnode(process);}
			if (displayInfo->isComposite == 1) {displayComposite(process);}
			if (displayInfo->outputTXT == 1) {writeCompositeTXT(process);}
			if (displayInfo->outputBIN == 1) {writeCompositeBIN(process);}
		}
	}
	else {
		PROCESS* process = getProcess(displayInfo->pid);

		if (displayInfo->isProcessFD == 1) {displayProcessFD(process);}
		if (displayInfo->isSystemWide == 1) {displaySystemWide(process);}
		if (displayInfo->isVnode == 1) {displayVnode(process);}
		if (displayInfo->isComposite == 1) {displayComposite(process);}
		if (displayInfo->isSummary == 1) {displaySummary(allProcesses, processCount);}
		if (displayInfo->threshold != NOTHRESHOLD) {displayOffending(allProcesses, processCount, displayInfo->threshold);}
		if (displayInfo->outputTXT == 1) {writeCompositeTXT(process);}
		if (displayInfo->outputBIN == 1) {writeCompositeBIN(process);}
		freeAllPROCESS(&process, 1);
	}

	freeAllPROCESS(allProcesses, processCount);

	
}


int getThreshold(char* arg) {
	if (strlen(arg) > 12) {
		arg[11] = '\0';
	}
	else {
		return NOTHING;
	}

	if (strcmp(arg, "--threshold") != 0) {
		return NOTHING;
	}

	return strtol(arg+12, NULL, 10);
}

DISPLAYINFO* processArguments(int argc, char** argv) {
	DISPLAYINFO* displayInfo = createDISPLAYINFO();

	if (argc == 1) {
		displayInfo->isComposite = 1;
		displayInfo->pid = ALLPID;
	}
	else if (argc == 2 && strtol(argv[1], NULL, 10) > 0) {
		displayInfo->pid = strtol(argv[1], NULL, 10);
		displayInfo->isProcessFD = 1;
	    displayInfo->isSystemWide = 1;
	    displayInfo->isVnode = 1;
	    displayInfo->isComposite = 1;
	}
	else {
		for (int i = 1; i < argc; i++) {
			if (strcmp(argv[i], "--per-process") == 0 ) {
				displayInfo->isProcessFD = 1;
			}
			else if (strcmp(argv[i], "--systemWide") == 0 ) {
				displayInfo->isSystemWide = 1;
			}
			else if (strcmp(argv[i], "--Vnodes") == 0 ) {
				displayInfo->isVnode = 1;
			}
			else if (strcmp(argv[i], "--composite") == 0 ) {
				displayInfo->isComposite = 1;
			}
			else if (strcmp(argv[i], "--summary") == 0 ) {
				displayInfo->isSummary = 1;
			}
			else if (i == 0 && strtol(argv[i], NULL, 10) > 0) {
				displayInfo->pid = strtol(argv[i], NULL, 10);
			}
			else if (getThreshold(argv[i]) != NOTHING) {
				displayInfo->threshold = getThreshold(argv[i]);
			}
			else {
				fprintf(stderr, "Error: invalid arguments");
				exit(1);
			}
		}
	}

	return displayInfo;

}



int test(int argc, char** argv) {

	DISPLAYINFO* displayInfo = processArguments(argc, argv);

	if (validProcess(displayInfo->pid)) {
		display(displayInfo);
	}
	else {
		fprintf(stderr, "Error: invalid processID");
		exit(1);
	}

	freeDISPLAYINFO(displayInfo);

	return 0;
}

int main() {

	return 0;
}