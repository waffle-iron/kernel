#include <inodes.h>


int TMPFS_Read(kInode_t* fp, void* buffer, size_t count, size_t lba)
{
  memset (buffer, 0, count * PAGE_SIZE);
  return 0;
}


int TMPFS_Create (const char* name, kInode_t* dir, kStat_t* file)
{
  time_t now = time(NULL);
  file->atime_ = now;
  file->mtime_ = now;
  file->ctime_ = now;
  file->dblock_ = 1;
  file->cblock_ = PAGE_SIZE;
  return 0;
}


kFileOp_t tmpFsOperation = {
  NULL, NULL, NULL, 
  NULL, TMPFS_Read, NULL, NULL, 
  TMPFS_Create, NULL, NULL, NULL, NULL, 
};

