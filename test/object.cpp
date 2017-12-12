// Copyright (c) 2014 BitPay Inc.
// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <cassert>
#include <stdexcept>
#include <univalue.h>
#include <iostream>

#define CHECK(expr) assert(expr)
#define CHECK_EQUAL(v1, v2) { \
    if (v1 != v2) { \
        std::cout << v1 << "!=" << v2; \
    } \
    assert((v1) == (v2)); \
}
#define CHECK_THROW(stmt, excMatch) { \
        try { \
            (stmt); \
        } catch (excMatch & e) { \
	} catch (...) { \
	    assert(0); \
	} \
    }
#define CHECK_NO_THROW(stmt) { \
        try { \
            (stmt); \
	} catch (...) { \
	    assert(0); \
	} \
    }


void univalue_constructor()
{
    UniValue v1;
    CHECK(v1.isNull());

    UniValue v2(UniValue::VSTR);
    CHECK(v2.isStr());

    UniValue v3(UniValue::VSTR, "foo");
    CHECK(v3.isStr());
    CHECK_EQUAL(v3.getValStr(), "foo");

    UniValue numTest;
    CHECK(numTest.setNumStr("82"));
    CHECK(numTest.isNum());
    CHECK_EQUAL(numTest.getValStr(), "82");

    uint64_t vu64 = 82;
    UniValue v4(vu64);
    CHECK(v4.isNum());
    CHECK_EQUAL(v4.getValStr(), "82");

    int64_t vi64 = -82;
    UniValue v5(vi64);
    CHECK(v5.isNum());
    CHECK_EQUAL(v5.getValStr(), "-82");

    int vi = -688;
    UniValue v6(vi);
    CHECK(v6.isNum());
    CHECK_EQUAL(v6.getValStr(), "-688");

    double vd = -7.21;
    UniValue v7(vd);
    CHECK(v7.isNum());
    CHECK_EQUAL(v7.getValStr(), "-7.21");

    std::string vs("yawn");
    UniValue v8(vs);
    CHECK(v8.isStr());
    CHECK_EQUAL(v8.getValStr(), "yawn");

    const char *vcs = "zappa";
    UniValue v9(vcs);
    CHECK(v9.isStr());
    CHECK_EQUAL(v9.getValStr(), "zappa");
}

void univalue_typecheck()
{
    UniValue v1;
    CHECK(v1.setNumStr("1"));
    CHECK(v1.isNum());
    CHECK_THROW(v1.get_bool(), std::runtime_error);

    UniValue v2;
    CHECK(v2.setBool(true));
    CHECK_EQUAL(v2.get_bool(), true);
    CHECK_THROW(v2.get_int(), std::runtime_error);

    UniValue v3;
    CHECK(v3.setNumStr("32482348723847471234"));
    CHECK_THROW(v3.get_int64(), std::runtime_error);
    CHECK(v3.setNumStr("1000"));
    CHECK_EQUAL(v3.get_int64(), 1000);

    UniValue v4;
    CHECK(v4.setNumStr("2147483648"));
    CHECK_EQUAL(v4.get_int64(), 2147483648);
    CHECK_THROW(v4.get_int(), std::runtime_error);
    CHECK(v4.setNumStr("1000"));
    CHECK_EQUAL(v4.get_int(), 1000);
    CHECK_THROW(v4.get_str(), std::runtime_error);
    CHECK_EQUAL(v4.get_real(), 1000);
    CHECK_THROW(v4.get_array(), std::runtime_error);
    CHECK_THROW(v4.getKeys(), std::runtime_error);
    CHECK_THROW(v4.getValues(), std::runtime_error);
    CHECK_THROW(v4.get_obj(), std::runtime_error);

    UniValue v5;
    CHECK(v5.read("[true, 10]"));
    CHECK_NO_THROW(v5.get_array());
    std::vector<UniValue> vals = v5.getValues();
    CHECK_THROW(vals[0].get_int(), std::runtime_error);
    CHECK_EQUAL(vals[0].get_bool(), true);

    CHECK_EQUAL(vals[1].get_int(), 10);
    CHECK_THROW(vals[1].get_bool(), std::runtime_error);
}

void univalue_set()
{
    UniValue v(UniValue::VSTR, "foo");
    v.clear();
    CHECK(v.isNull());
    CHECK_EQUAL(v.getValStr(), "");

    CHECK(v.setObject());
    CHECK(v.isObject());
    CHECK_EQUAL(v.size(), 0);
    CHECK_EQUAL(v.getType(), UniValue::VOBJ);
    CHECK(v.empty());

    CHECK(v.setArray());
    CHECK(v.isArray());
    CHECK_EQUAL(v.size(), 0);

    CHECK(v.setStr("zum"));
    CHECK(v.isStr());
    CHECK_EQUAL(v.getValStr(), "zum");

    CHECK(v.setFloat(-1.01));
    CHECK(v.isNum());
    CHECK_EQUAL(v.getValStr(), "-1.01");

    CHECK(v.setInt((int)1023));
    CHECK(v.isNum());
    CHECK_EQUAL(v.getValStr(), "1023");

    CHECK(v.setInt((int64_t)-1023LL));
    CHECK(v.isNum());
    CHECK_EQUAL(v.getValStr(), "-1023");

    CHECK(v.setInt((uint64_t)1023ULL));
    CHECK(v.isNum());
    CHECK_EQUAL(v.getValStr(), "1023");

    CHECK(v.setNumStr("-688"));
    CHECK(v.isNum());
    CHECK_EQUAL(v.getValStr(), "-688");

    CHECK(v.setBool(false));
    CHECK_EQUAL(v.isBool(), true);
    CHECK_EQUAL(v.isTrue(), false);
    CHECK_EQUAL(v.isFalse(), true);
    CHECK_EQUAL(v.getBool(), false);

    CHECK(v.setBool(true));
    CHECK_EQUAL(v.isBool(), true);
    CHECK_EQUAL(v.isTrue(), true);
    CHECK_EQUAL(v.isFalse(), false);
    CHECK_EQUAL(v.getBool(), true);

    CHECK(!v.setNumStr("zombocom"));

    CHECK(v.setNull());
    CHECK(v.isNull());
}

void univalue_array()
{
    UniValue arr(UniValue::VARR);

    UniValue v((int64_t)1023LL);
    CHECK(arr.push_back(v));

    std::string vStr("zippy");
    CHECK(arr.push_back(vStr));

    const char *s = "pippy";
    CHECK(arr.push_back(s));

    std::vector<UniValue> vec;
    v.setStr("boing");
    vec.push_back(v);

    v.setStr("going");
    vec.push_back(v);

    CHECK(arr.push_backV(vec));

    CHECK(arr.push_back((uint64_t) 400ULL));
    CHECK(arr.push_back((int64_t) -400LL));
    CHECK(arr.push_back((int) -401));
    CHECK(arr.push_back(-40.1));

    CHECK_EQUAL(arr.empty(), false);
    CHECK_EQUAL(arr.size(), 9);

    CHECK_EQUAL(arr[0].getValStr(), "1023");
    CHECK_EQUAL(arr[1].getValStr(), "zippy");
    CHECK_EQUAL(arr[2].getValStr(), "pippy");
    CHECK_EQUAL(arr[3].getValStr(), "boing");
    CHECK_EQUAL(arr[4].getValStr(), "going");
    CHECK_EQUAL(arr[5].getValStr(), "400");
    CHECK_EQUAL(arr[6].getValStr(), "-400");
    CHECK_EQUAL(arr[7].getValStr(), "-401");
    CHECK_EQUAL(arr[8].getValStr(), "-40.1");

    CHECK_EQUAL(arr[999].getValStr(), "");

    arr.clear();
    CHECK(arr.empty());
    CHECK_EQUAL(arr.size(), 0);
}

void univalue_object()
{
    UniValue obj(UniValue::VOBJ);
    std::string strKey, strVal;
    UniValue v;

    strKey = "age";
    v.setInt(100);
    CHECK(obj.pushKV(strKey, v));

    strKey = "first";
    strVal = "John";
    CHECK(obj.pushKV(strKey, strVal));

    strKey = "last";
    const char *cVal = "Smith";
    CHECK(obj.pushKV(strKey, cVal));

    strKey = "distance";
    CHECK(obj.pushKV(strKey, (int64_t) 25));

    strKey = "time";
    CHECK(obj.pushKV(strKey, (uint64_t) 3600));

    strKey = "calories";
    CHECK(obj.pushKV(strKey, (int) 12));

    strKey = "temperature";
    CHECK(obj.pushKV(strKey, (double) 90.012));

    strKey = "moon";
    CHECK(obj.pushKV(strKey, true));

    strKey = "spoon";
    CHECK(obj.pushKV(strKey, false));

    UniValue obj2(UniValue::VOBJ);
    CHECK(obj2.pushKV("cat1", 9000));
    CHECK(obj2.pushKV("cat2", 12345));

    CHECK(obj.pushKVs(obj2));

    CHECK_EQUAL(obj.empty(), false);
    CHECK_EQUAL(obj.size(), 11);

    CHECK_EQUAL(obj["age"].getValStr(), "100");
    CHECK_EQUAL(obj["first"].getValStr(), "John");
    CHECK_EQUAL(obj["last"].getValStr(), "Smith");
    CHECK_EQUAL(obj["distance"].getValStr(), "25");
    CHECK_EQUAL(obj["time"].getValStr(), "3600");
    CHECK_EQUAL(obj["calories"].getValStr(), "12");
    CHECK_EQUAL(obj["temperature"].getValStr(), "90.012");
    CHECK_EQUAL(obj["moon"].getValStr(), "true");
    CHECK_EQUAL(obj["spoon"].getValStr(), "false");
    CHECK_EQUAL(obj["cat1"].getValStr(), "9000");
    CHECK_EQUAL(obj["cat2"].getValStr(), "12345");

    CHECK_EQUAL(obj["nyuknyuknyuk"].getValStr(), "");

    CHECK(obj.exists("age"));
    CHECK(obj.exists("first"));
    CHECK(obj.exists("last"));
    CHECK(obj.exists("distance"));
    CHECK(obj.exists("time"));
    CHECK(obj.exists("calories"));
    CHECK(obj.exists("temperature"));
    CHECK(obj.exists("moon"));
    CHECK(obj.exists("spoon"));
    CHECK(obj.exists("cat1"));
    CHECK(obj.exists("cat2"));

    CHECK(!obj.exists("nyuknyuknyuk"));

    std::map<std::string, UniValue::VType> objTypes;
    objTypes["age"] = UniValue::VNUM;
    objTypes["first"] = UniValue::VSTR;
    objTypes["last"] = UniValue::VSTR;
    objTypes["distance"] = UniValue::VNUM;
    objTypes["time"] = UniValue::VNUM;
    objTypes["calories"] = UniValue::VNUM;
    objTypes["temperature"] = UniValue::VNUM;
    objTypes["moon"] = UniValue::VBOOL;
    objTypes["spoon"] = UniValue::VBOOL;
    objTypes["cat1"] = UniValue::VNUM;
    objTypes["cat2"] = UniValue::VNUM;
    CHECK(obj.checkObject(objTypes));

    objTypes["cat2"] = UniValue::VSTR;
    CHECK(!obj.checkObject(objTypes));

    obj.clear();
    CHECK(obj.empty());
    CHECK_EQUAL(obj.size(), 0);
    CHECK_EQUAL(obj.getType(), UniValue::VNULL);

    CHECK_EQUAL(obj.setObject(), true);
    UniValue uv;
    uv.setInt(42);
    obj.__pushKV("age", uv);
    CHECK_EQUAL(obj.size(), 1);
    CHECK_EQUAL(obj["age"].getValStr(), "42");

    uv.setInt(43);
    obj.pushKV("age", uv);
    CHECK_EQUAL(obj.size(), 1);
    CHECK_EQUAL(obj["age"].getValStr(), "43");

    obj.pushKV("name", "foo bar");

    std::map<std::string,UniValue> kv;
    obj.getObjMap(kv);
    CHECK_EQUAL(kv["age"].getValStr(), "43");
    CHECK_EQUAL(kv["name"].getValStr(), "foo bar");

}

static const char *json1 =
"[1.10000000,{\"key1\":\"str\\u0000\",\"key2\":800,\"key3\":{\"name\":\"martian http://test.com\"}}]";

void univalue_readwrite()
{
    UniValue v;
    CHECK(v.read(json1));

    std::string strJson1(json1);
    CHECK(v.read(strJson1));

    CHECK(v.isArray());
    CHECK_EQUAL(v.size(), 2);

    CHECK_EQUAL(v[0].getValStr(), "1.10000000");

    UniValue obj = v[1];
    CHECK(obj.isObject());
    CHECK_EQUAL(obj.size(), 3);

    CHECK(obj["key1"].isStr());
    std::string correctValue("str");
    correctValue.push_back('\0');
    CHECK_EQUAL(obj["key1"].getValStr(), correctValue);
    CHECK(obj["key2"].isNum());
    CHECK_EQUAL(obj["key2"].getValStr(), "800");
    CHECK(obj["key3"].isObject());

    CHECK_EQUAL(strJson1, v.write());

    /* Check for (correctly reporting) a parsing error if the initial
       JSON construct is followed by more stuff.  Note that whitespace
       is, of course, exempt.  */

    CHECK(v.read("  {}\n  "));
    CHECK(v.isObject());
    CHECK(v.read("  []\n  "));
    CHECK(v.isArray());

    CHECK(!v.read("@{}"));
    CHECK(!v.read("{} garbage"));
    CHECK(!v.read("[]{}"));
    CHECK(!v.read("{}[]"));
    CHECK(!v.read("{} 42"));
}

int main (int argc, char *argv[])
{
    univalue_constructor();
    univalue_typecheck();
    univalue_set();
    univalue_array();
    univalue_object();
    univalue_readwrite();
    return 0;
}

