#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

//Constructor - create instances of file, readPage, writePage and record
DBFile::DBFile() {
    this->file = new File();
    this->readPage = new Page();
    this->writePage = new Page();
    this->currentRecord = new Record();
}

//Destructor
DBFile::~DBFile(){
    delete file;
    delete readPage;
    delete writePage;
    delete currentRecord;
}

//Move to the first record
void DBFile::MoveFirst () {
    if(this->file->GetLength() != 0) {
        this->file->GetPage(this->readPage, 1);
    }
    else {
        this->readPage->EmptyItOut();
    }
}

//Add a record
void DBFile::Add (Record &rec) {
    this->dirtyWrite=true;
    Record recordToAdd;
    recordToAdd.Consume(&rec);
    if(writePage->Append(&recordToAdd)==0)
    {
        this->file->AddPage(writePage,writeOffset);
        writeOffset++;
        this->writePage->EmptyItOut();
        writePage->Append(&recordToAdd);
    }
}

//Get next record
int DBFile::GetNext (Record &fetchme) {
    if (eof) {
        return 0;
    }
    int fetchStatus = this->readPage->GetFirst(&fetchme);
    if (fetchStatus == 0) {
        pageOffset += 1;
        if (pageOffset + 1 <= this->file->GetLength() - 1) {
            this->file->GetPage(this->readPage, pageOffset);
            this->readPage->GetFirst(&fetchme);
            return 1;
        } else {
            return 0;
        }
    }
        return 1;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
    ComparisonEngine compareRecords;
    while(this->GetNext(fetchme) == 1) {
        if(compareRecords.Compare(&fetchme,&literal,&cnf) == 1) {
            return 1;
        }
        else {
            continue;
        }
    }
    return 0;
}

//Create a dbfile of type heap
int DBFile::Create (const char *f_path, fType f_type, void *startup) {
    this->file->Open(0,const_cast<char *>(f_path));
    pageOffset = 1;
    writeOffset = 1;
    dirtyWrite = false;
    eof = true;
    return 1;
}

//Open DBfile
int DBFile::Open (const char *f_path) {
    this->file->Open(1,const_cast<char *>(f_path));
    pageOffset = 1;
    eof = false;
    return 1;
}

//Close DBfile
int DBFile::Close () {
        if (this->dirtyWrite) {
            this->file->AddPage(writePage, writeOffset);
            writeOffset++;
        }
        eof = true;
        return this->file->Close();
}

//Load schema
void DBFile::Load (Schema &f_schema, const char *loadpath) {
    FILE* loadFile = fopen (loadpath,"r");
    Record recordToLoad;
    while(recordToLoad.SuckNextRecord(&f_schema,loadFile)!=0)
        this->Add(recordToLoad);
    fclose(loadFile);
}



