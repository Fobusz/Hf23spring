//
// Created by Tomi on 2023. 05. 28..
//

#include "gtest_lite.h"
#include "konf_hash.hpp"
#include "menu.h"

#define ELKESZULT 9



int main() {

    KonfHash k1;
    k1.readFile("../input.txt");

#if ELKESZULT >= 0
    {
        /**
         * It tests whether the post function is working properly with proper values
         */
        TEST(postTest, test1){
            k1.post(make_pair("alma","korte"));
            EXPECT_STREQ("korte", k1.get<string>("alma").c_str());
        }END

        /**
         * It tests whether the post function is working properly if we want to post again with the same key
         */
        TEST(postTest, test2){
            k1.post(make_pair("alma","alma"));
            EXPECT_STREQ("korte", k1.get<string>("alma").c_str());
        }END

        /**
         * It tests whether the post function is working properly if we post an integer value
         */
        TEST(postTest, test3){
            k1.post(make_pair("egy","1"));
            EXPECT_EQ(1, k1.get<int>("egy"));
        }END

        /**
         * It tests whether the post function is working properly if we post a double value
         */
        TEST(postTest, test4){
            k1.post(make_pair("repulo","12.3"));
            EXPECT_EQ(12.3, k1.get<double>("repulo"));
        }END

        /**
         * It tests whether the post function is working properly if we post a boolean value
         */
        TEST(postTest, test5){
            k1.post(make_pair("elem","true"));
            EXPECT_EQ(true, k1.get<bool>("elem"));
        }END

        /**
         * It tests whether the post function is working properly if we use a blank string as key
         */
        TEST(postTest, test6){
                k1.post(make_pair("","true"));
                EXPECT_EQ(true, k1.get<bool>(""));
            }END

        /**
        * It tests whether the post function is working properly if we use a blank string as value
        */
        TEST(postTest, test6){
            k1.post(make_pair("bagoly",""));
            EXPECT_STREQ("", k1.get<string>("bagoly").c_str());
        }END

        /**
         * It tests whether the put function is working properly with proper inputs
         */
        TEST(putTest, test1){
            k1.post(make_pair("tehen","korte"));
            k1.put(make_pair("tehen","alma"));
            EXPECT_STREQ("alma", k1.get<string>("tehen").c_str());
        }END

        /**
         * It tests whether the put function is working properly with proper inputs
         */
        TEST(putTest, test2){
            k1.post(make_pair("almasahdljk","korte"));
            k1.put(make_pair("almasahdljk","4"));
            EXPECT_EQ(4, k1.get<int>("almasahdljk"));
        }END

        /**
        * It tests whether the put function is working properly with incorrect inputs
        */
        TEST(putTest, test3){
                k1.post(make_pair("almakorte","korte"));
                k1.put(make_pair("almabarack","4"));
                EXPECT_EQ(-1, k1.get<int>("almabarack"));
            }END

        /**
         * It tests whether the del function is working properly with proper inputs
         */
        TEST(delTest, test1){
            k1.post(make_pair("almasahdljkaa","korte"));
            k1.del("almasahdljkaa");
            EXPECT_EQ(-1, k1.get<int>("almasahdljkaa"));
        }END

        /**
         * It tests whether the del function is working properly with incorrect inputs
         */
        TEST(delTest, test1){
            k1.post(make_pair("dfsadasf","3"));
            k1.del("dfsadasfdasdsa");
            EXPECT_EQ(3, k1.get<int>("dfsadasf"));
        }END

    }  // blokk vége
#endif // ELKESZULT >= 0

    return 0;
}
