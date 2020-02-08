#include <gtest/gtest.h>
#include "DBFile.h"
#include "test.h"

// make sure that the file path/dir information below is correct
const char *dbfile_dir = ""; // dir where binary heap files should be stored
const char *tpch_dir ="/home/bhaasita/dev/DBI/P1/git/tpch-dbgen/"; // dir where dbgen tpch files (extension *.tbl) can be found
const char *catalog_path = "catalog"; // full path of the catalog file

using namespace std;

relation *rel;

//Test to demonstrate successful create
TEST(DBFileTest1, CreateTest1) {
    DBFile dbFileTest;
    ASSERT_EQ(dbFileTest.Create(li->path(), heap, NULL), 1);
}

//Test to demonstrate create on failure
TEST(DBFileTest2, CreateTest2) {
    DBFile dbFileTest;
    EXPECT_EXIT( dbFileTest.Create( "", heap, NULL), ::testing::ExitedWithCode(1), "BAD.*");
}

//Test to demonstrate successful open
TEST(DBFileTest3, OpenTest1) {
    DBFile dbFileTest;
    ASSERT_EQ(dbFileTest.Open(li->path()), 1);
}

//Test to demonstrate open on failure
TEST(DBFileTest4, OpenTest2) {
    DBFile dbFileTest;
    EXPECT_EXIT( dbFileTest.Open("randomTestFile.bin"), ::testing::ExitedWithCode(1), "BAD.*");
}

//Test to demonstrate successful close
TEST(DBFileTest5, CloseFile) {
    DBFile dbFileTest;
    dbFileTest.Create(li->path(), heap, NULL);
    char tbl_path[100]; // construct path of the tpch flat text file
    sprintf (tbl_path, "%s%s.tbl", tpch_dir, li->name());
    dbFileTest.Load (*(li->schema ()), tbl_path);
    ASSERT_GE(dbFileTest.Close(), 0);
}

//Run all tests
int main(int argc, char **argv) {
    setup (catalog_path, dbfile_dir, tpch_dir);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
