
//此源码被清华学神尹成大魔王专业翻译分析并修改
//尹成QQ77025077
//尹成微信18510341407
//尹成所在QQ群721929980
//尹成邮箱 yinc13@mails.tsinghua.edu.cn
//尹成毕业于清华大学,微软区块链领域全球最有价值专家
//https://mvp.microsoft.com/zh-cn/PublicProfile/4033620
//————————————————————————————————————————————————————————————————————————————————————————————————————————————————
/*
    此文件是Rippled的一部分：https://github.com/ripple/rippled
    版权所有（c）2016 Ripple Labs Inc.

    使用、复制、修改和/或分发本软件的权限
    特此授予免费或不收费的目的，前提是
    版权声明和本许可声明出现在所有副本中。

    本软件按“原样”提供，作者不作任何保证。
    关于本软件，包括
    适销性和适用性。在任何情况下，作者都不对
    任何特殊、直接、间接或后果性损害或任何损害
    因使用、数据或利润损失而导致的任何情况，无论是在
    合同行为、疏忽或其他侵权行为
    或与本软件的使用或性能有关。
**/

//==============================================================

#include <ripple/protocol/JsonFields.h>
#include <ripple/json/json_value.h>
#include <ripple/json/to_string.h>
#include <ripple/json/json_reader.h>
#include <test/jtx.h>

#include <boost/utility/string_ref.hpp>

namespace ripple {
namespace test {

static char const* bobs_account_objects[] = {
R"json({
  "Account" : "rPMh7Pi9ct699iZUTWaytJUoHcJ7cgyziK",
  "BookDirectory" : "50AD0A9E54D2B381288D535EB724E4275FFBF41580D28A925D038D7EA4C68000",
  "BookNode" : "0000000000000000",
  "Flags" : 65536,
  "LedgerEntryType" : "Offer",
  "OwnerNode" : "0000000000000000",
  "Sequence" : 4,
  "TakerGets" : {
    "currency" : "USD",
    "issuer" : "rPMh7Pi9ct699iZUTWaytJUoHcJ7cgyziK",
    "value" : "1"
  },
  "TakerPays" : "100000000",
  "index" : "A984D036A0E562433A8377CA57D1A1E056E58C0D04818F8DFD3A1AA3F217DD82"
})json"
,
R"json({
    "Account" : "rPMh7Pi9ct699iZUTWaytJUoHcJ7cgyziK",
    "BookDirectory" : "B025997A323F5C3E03DDF1334471F5984ABDE31C59D463525D038D7EA4C68000",
    "BookNode" : "0000000000000000",
    "Flags" : 65536,
    "LedgerEntryType" : "Offer",
    "OwnerNode" : "0000000000000000",
    "Sequence" : 5,
    "TakerGets" : {
        "currency" : "USD",
        "issuer" : "r32rQHyesiTtdWFU7UJVtff4nCR5SHCbJW",
        "value" : "1"
    },
    "TakerPays" : "100000000",
    "index" : "CAFE32332D752387B01083B60CC63069BA4A969C9730836929F841450F6A718E"
})json"
,
R"json({
    "Balance" : {
        "currency" : "USD",
        "issuer" : "rrrrrrrrrrrrrrrrrrrrBZbvji",
        "value" : "-1000"
    },
    "Flags" : 131072,
    "HighLimit" : {
        "currency" : "USD",
        "issuer" : "rPMh7Pi9ct699iZUTWaytJUoHcJ7cgyziK",
        "value" : "1000"
    },
    "HighNode" : "0000000000000000",
    "LedgerEntryType" : "RippleState",
    "LowLimit" : {
        "currency" : "USD",
        "issuer" : "r9cZvwKU3zzuZK9JFovGg1JC5n7QiqNL8L",
        "value" : "0"
    },
    "LowNode" : "0000000000000000",
    "index" : "D13183BCFFC9AAC9F96AEBB5F66E4A652AD1F5D10273AEB615478302BEBFD4A4"
})json"
,
R"json({
    "Balance" : {
        "currency" : "USD",
        "issuer" : "rrrrrrrrrrrrrrrrrrrrBZbvji",
        "value" : "-1000"
    },
    "Flags" : 131072,
    "HighLimit" : {
        "currency" : "USD",
        "issuer" : "rPMh7Pi9ct699iZUTWaytJUoHcJ7cgyziK",
        "value" : "1000"
    },
    "HighNode" : "0000000000000000",
    "LedgerEntryType" : "RippleState",
    "LowLimit" : {
        "currency" : "USD",
        "issuer" : "r32rQHyesiTtdWFU7UJVtff4nCR5SHCbJW",
        "value" : "0"
    },
    "LowNode" : "0000000000000000",
    "index" : "D89BC239086183EB9458C396E643795C1134963E6550E682A190A5F021766D43"
})json"
};

class AccountObjects_test : public beast::unit_test::suite
{
public:
    void testErrors()
    {
        testcase("error cases");

        using namespace jtx;
        Env env(*this);

//无账户测试错误
        {
            auto resp = env.rpc("json", "account_objects");
            BEAST_EXPECT( resp[jss::error_message] ==
                "Syntax error.");
        }
//错误的帐户字符串上的测试错误。
        {
            Json::Value params;
            params[jss::account] = "n94JNrQYkDrpt62bbSR7nVEhdyAvcJXRAsjEkFYyqRkh9SUTYEqV";
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Disallowed seed.");
        }
//测试不在分类帐中的帐户错误。
        {
            Json::Value params;
            params[jss::account] = Account{ "bogie" }.human();
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Account not found.");
        }
        Account const bob{ "bob" };
//大分类帐索引上的测试错误。
        {
            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::ledger_index] = 10;
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "ledgerNotFound");
        }

        env.fund(XRP(1000), bob);
//类型参数上的测试错误不是字符串
        {
            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::type] = 10;
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] ==
                "Invalid field 'type', not string.");
        }
//类型参数上的测试错误不是有效类型
        {
            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::type] = "expedited";
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'type'.");
        }
//极限测试误差-ve
        {
            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::limit] = -1;
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] ==
                "Invalid field 'limit', not unsigned integer.");
        }
//标记上的测试错误
        {
            Account const gw{ "G" };
            env.fund(XRP(1000), gw);
            auto const USD = gw["USD"];
            env.trust(USD(1000), bob);
            env(pay(gw, bob, XRP(1)));
            env(offer(bob, XRP(100), bob["USD"](1)), txflags(tfPassive));

            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::limit] = 1;
            auto resp = env.rpc("json", "account_objects", to_string(params));

            auto resume_marker = resp[jss::result][jss::marker];
            std::string mark = to_string(resume_marker);
            params[jss::marker] = 10;
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'marker', not string.");

            params[jss::marker] = "This is a string with no comma";
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'marker'.");

            params[jss::marker] = "This string has a comma, but is not hex";
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'marker'.");

            params[jss::marker] = std::string(&mark[1U], 64);
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'marker'.");

            params[jss::marker] = std::string(&mark[1U], 65);
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'marker'.");

            params[jss::marker] = std::string(&mark[1U], 65) + "not hex";
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::error_message] == "Invalid field 'marker'.");

//这应该是个错误吗？
//标记末尾缺少十六进制数字。
//未返回帐户对象。
            params[jss::marker] = std::string(&mark[1U], 128);
            resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( resp[jss::result][jss::account_objects].size() == 0);
        }
    }

    void testUnsteppedThenStepped()
    {
        testcase("unsteppedThenStepped");

        using namespace jtx;
        Env env(*this);

        Account const gw1{ "G1" };
        Account const gw2{ "G2" };
        Account const bob{ "bob" };

        auto const USD1 = gw1["USD"];
        auto const USD2 = gw2["USD"];

        env.fund(XRP(1000), gw1, gw2, bob);
        env.trust(USD1(1000), bob);
        env.trust(USD2(1000), bob);

        env(pay(gw1, bob, USD1(1000)));
        env(pay(gw2, bob, USD2(1000)));

        env(offer(bob, XRP(100), bob["USD"](1)),txflags(tfPassive));
        env(offer(bob, XRP(100), USD1(1)), txflags(tfPassive));

        Json::Value bobj[4];
        for (int i = 0; i < 4; ++i)
            Json::Reader{}.parse(bobs_account_objects[i], bobj[i]);

//测试“未测试”
//即请求没有明确限制/标记分页的帐户对象
        {
            Json::Value params;
            params[jss::account] = bob.human();
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( !resp.isMember(jss::marker));

            BEAST_EXPECT( resp[jss::result][jss::account_objects].size() == 4);
            for (int i = 0; i < 4; ++i)
            {
                auto& aobj = resp[jss::result][jss::account_objects][i];
                aobj.removeMember("PreviousTxnID");
                aobj.removeMember("PreviousTxnLgrSeq");

                if (aobj != bobj[i])
                   std::cout << "Fail at " << i << ": " << aobj << std::endl;
                BEAST_EXPECT(aobj == bobj[i]);
            }
        }
//类型参数为筛选器的测试请求，未替换
        {
            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::type] = "state";
            auto resp = env.rpc("json", "account_objects", to_string(params));
            BEAST_EXPECT( !resp.isMember(jss::marker));

            BEAST_EXPECT( resp[jss::result][jss::account_objects].size() == 2);
            for (int i = 0; i < 2; ++i)
            {
                auto& aobj = resp[jss::result][jss::account_objects][i];
                aobj.removeMember("PreviousTxnID");
                aobj.removeMember("PreviousTxnLgrSeq");

                BEAST_EXPECT( aobj == bobj[i+2]);
            }
        }
//一次一步测试，限值为1，从上一个标记恢复
        {
            Json::Value params;
            params[jss::account] = bob.human();
            params[jss::limit] = 1;
            for (int i = 0; i < 4; ++i)
            {
                auto resp = env.rpc("json", "account_objects", to_string(params));
                auto& aobjs = resp[jss::result][jss::account_objects];
                BEAST_EXPECT( aobjs.size() == 1);
                auto& aobj = aobjs[0U];
                if (i < 3) BEAST_EXPECT( resp[jss::result][jss::limit] == 1);

                aobj.removeMember("PreviousTxnID");
                aobj.removeMember("PreviousTxnLgrSeq");

                BEAST_EXPECT(aobj == bobj[i]);

                auto resume_marker = resp[jss::result][jss::marker];
                params[jss::marker] = resume_marker;
            }
        }
    }

    void testObjectTypes()
    {
        testcase("object types");

//给吉布达•伟士一堆分类账对象，确保我们能够检索到
//按类型分类。
        using namespace jtx;

        Account const alice { "alice" };
        Account const gw{ "gateway" };
        auto const USD = gw["USD"];

//在支持票据帐户对象时测试它们。
        Env env(*this, supported_amendments().set(featureTickets));

//制作一个lambda，我们可以用它轻松地获得“account_objects”。
        auto acct_objs = [&env] (Account const& acct, char const* type)
        {
            Json::Value params;
            params[jss::account] = acct.human();
            params[jss::type] = type;
            params[jss::ledger_index] = "validated";
            return env.rpc("json", "account_objects", to_string(params));
        };

//创建一个lambda，它可以轻松地标识帐户对象的大小。
        auto acct_objs_is_size = [](Json::Value const& resp, unsigned size) {
            return resp[jss::result][jss::account_objects].isArray() &&
                (resp[jss::result][jss::account_objects].size() == size);
        };

        env.fund(XRP(10000), gw, alice);
        env.close();

//由于帐户现在为空，所有帐户对象都应该
//回到空。
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::account), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::amendments), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::check), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::deposit_preauth), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::directory), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::escrow), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::fee), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::hashes), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::offer), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::payment_channel), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::signer_list), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::state), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::ticket), 0));

//建立一条信任线以便我们找到它。
        env.trust(USD(1000), alice);
        env.close();
        env(pay(gw, alice, USD(5)));
        env.close();
        {
//找到信任线并确保它是正确的。
            Json::Value const resp = acct_objs (gw, jss::state);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& state = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (state[sfBalance.jsonName][jss::value].asInt() == -5);
            BEAST_EXPECT (state[sfHighLimit.jsonName][jss::value].asUInt() == 1000);
        }
        {
//吉布达•伟士给爱丽丝开了一张10美元的支票。
            Json::Value jvCheck;
            jvCheck[sfAccount.jsonName] = gw.human();
            jvCheck[sfSendMax.jsonName] = USD(10).value().getJson(0);
            jvCheck[sfDestination.jsonName] = alice.human();
            jvCheck[sfTransactionType.jsonName] = "CheckCreate";
            jvCheck[sfFlags.jsonName] = tfUniversal;
            env (jvCheck);
            env.close();
        }
        {
//找到支票。
            Json::Value const resp = acct_objs (gw, jss::check);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& check = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (check[sfAccount.jsonName] == gw.human());
            BEAST_EXPECT (check[sfDestination.jsonName] == alice.human());
            BEAST_EXPECT (check[sfSendMax.jsonName][jss::value].asUInt() == 10);
        }
//吉布达•伟士预先授权艾丽斯付款。
        env (deposit::auth (gw, alice));
        env.close();
        {
//找到预授权。
            Json::Value const resp = acct_objs (gw, jss::deposit_preauth);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& preauth = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (preauth[sfAccount.jsonName] == gw.human());
            BEAST_EXPECT (preauth[sfAuthorize.jsonName] == alice.human());
        }
        {
//吉布达•伟士创建了一个我们可以在分类账中寻找的托管。
            Json::Value jvEscrow;
            jvEscrow[jss::TransactionType] = "EscrowCreate";
            jvEscrow[jss::Flags] = tfUniversal;
            jvEscrow[jss::Account] = gw.human();
            jvEscrow[jss::Destination] = gw.human();
            jvEscrow[jss::Amount] = XRP(100).value().getJson(0);
            jvEscrow[sfFinishAfter.jsonName] =
                env.now().time_since_epoch().count() + 1;
            env (jvEscrow);
            env.close();
        }
        {
//找到代管处。
            Json::Value const resp = acct_objs (gw, jss::escrow);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& escrow = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (escrow[sfAccount.jsonName] == gw.human());
            BEAST_EXPECT (escrow[sfDestination.jsonName] == gw.human());
            BEAST_EXPECT (escrow[sfAmount.jsonName].asUInt() == 100'000'000);
        }
//吉布达•伟士创建了一个我们可以在分类账中寻找的报价。
        env (offer (gw, USD (7), XRP (14)));
        env.close();
        {
//找到报价。
            Json::Value const resp = acct_objs (gw, jss::offer);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& offer = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (offer[sfAccount.jsonName] == gw.human());
            BEAST_EXPECT (offer[sfTakerGets.jsonName].asUInt() == 14'000'000);
            BEAST_EXPECT (offer[sfTakerPays.jsonName][jss::value].asUInt() == 7);
        }
        {
//创建一个从QW到Alice的付款渠道，我们可以查找。
            Json::Value jvPayChan;
            jvPayChan[jss::TransactionType] = "PaymentChannelCreate";
            jvPayChan[jss::Flags] = tfUniversal;
            jvPayChan[jss::Account] = gw.human ();
            jvPayChan[jss::Destination] = alice.human ();
            jvPayChan[jss::Amount] = XRP (300).value().getJson (0);
            jvPayChan[sfSettleDelay.jsonName] = 24 * 60 * 60;
            jvPayChan[sfPublicKey.jsonName] = strHex (gw.pk().slice ());
            env (jvPayChan);
            env.close();
        }
        {
//找到付款渠道。
            Json::Value const resp = acct_objs (gw, jss::payment_channel);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& payChan = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (payChan[sfAccount.jsonName] == gw.human());
            BEAST_EXPECT (payChan[sfAmount.jsonName].asUInt() == 300'000'000);
            BEAST_EXPECT (payChan[sfSettleDelay.jsonName].asUInt() == 24 * 60 * 60);
        }
//通过添加签名者列表使gw多点火。
        env (signers (gw, 6, { { alice, 7} }));
        env.close();
        {
//查找签名者列表。
            Json::Value const resp = acct_objs (gw, jss::signer_list);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& signerList = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (signerList[sfSignerQuorum.jsonName] == 6);
            auto const& entry =
                signerList[sfSignerEntries.jsonName][0u][sfSignerEntry.jsonName];
            BEAST_EXPECT (entry[sfAccount.jsonName] == alice.human());
            BEAST_EXPECT (entry[sfSignerWeight.jsonName].asUInt() == 7);
        }
//为吉布达•伟士制作门票。
        env (ticket::create (gw, gw));
        env.close();
        {
//找到票。
            Json::Value const resp = acct_objs (gw, jss::ticket);
            BEAST_EXPECT (acct_objs_is_size (resp, 1));

            auto const& ticket = resp[jss::result][jss::account_objects][0u];
            BEAST_EXPECT (ticket[sfAccount.jsonName] == gw.human());
            BEAST_EXPECT (ticket[sfLedgerEntryType.jsonName] == "Ticket");
            BEAST_EXPECT (ticket[sfSequence.jsonName].asUInt() == 9);
        }
//运行目录条目的数量，使gw有两个
//目录节点。
        for (int d = 1'000'032; d >= 1'000'000; --d)
        {
            env (offer (gw, USD (1), drops (d)));
            env.close();
        }

//验证不返回的类型是否仍然不返回任何内容。
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::account), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::amendments), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::directory), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::fee), 0));
        BEAST_EXPECT (acct_objs_is_size (acct_objs (gw, jss::hashes), 0));
    }

    void run() override
    {
        testErrors();
        testUnsteppedThenStepped();
        testObjectTypes();
    }
};

BEAST_DEFINE_TESTSUITE(AccountObjects,app,ripple);

}
}
