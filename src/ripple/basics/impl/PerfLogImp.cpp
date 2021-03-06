
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
    版权所有（c）2018 Ripple Labs Inc.

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

#include <ripple/basics/BasicConfig.h>
#include <ripple/basics/impl/PerfLogImp.h>
#include <ripple/beast/core/CurrentThreadName.h>
#include <ripple/beast/utility/Journal.h>
#include <ripple/json/json_writer.h>
#include <ripple/json/to_string.h>
#include <boost/optional.hpp>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

namespace ripple {
namespace perf {

PerfLogImp::Counters::Counters(std::vector<char const*> const& labels,
    JobTypes const& jobTypes)
{
    {
//平民党
        rpc_.reserve(labels.size());
        for (std::string const label : labels)
        {
            auto const elem = rpc_.emplace(label, Rpc());
            if (!elem.second)
            {
//确保没有其他函数填充此项。
                assert(false);
            }
        }
    }
    {
//人口JQ
        jq_.reserve(jobTypes.size());
        for (auto const& job : jobTypes)
        {
            auto const elem = jq_.emplace(job.first, Jq(job.second.name()));
            if (!elem.second)
            {
//确保没有其他函数填充此项。
                assert(false);
            }
        }
    }
}

Json::Value
PerfLogImp::Counters::countersJson() const
{
    Json::Value rpcobj(Json::objectValue);
//TotalRPC表示所有RPC方法。所有的开始、结束等等。
    Rpc totalRpc;
    for (auto const& proc : rpc_)
    {
        Json::Value p(Json::objectValue);
        {
            auto const sync =
                [&proc]() ->boost::optional<Counters::Rpc::Sync> {
                std::lock_guard<std::mutex> lock(proc.second.mut);
                if (!proc.second.sync.started &&
                    !proc.second.sync.finished &&
                    !proc.second.sync.errored)
                {
                    return boost::none;
                }
                return proc.second.sync;
            }();
            if (!sync)
                continue;

            p[jss::started] = std::to_string(sync->started);
            totalRpc.sync.started += sync->started;
            p[jss::finished] = std::to_string(sync->finished);
            totalRpc.sync.finished += sync->finished;
            p[jss::errored] = std::to_string(sync->errored);
            totalRpc.sync.errored += sync->errored;
            p[jss::duration_us] = std::to_string(sync->duration.count());
            totalRpc.sync.duration += sync->duration;
        }
        rpcobj[proc.first] = p;
    }

    if (totalRpc.sync.started)
    {
        Json::Value totalRpcJson(Json::objectValue);
        totalRpcJson[jss::started] = std::to_string(totalRpc.sync.started);
        totalRpcJson[jss::finished] = std::to_string(totalRpc.sync.finished);
        totalRpcJson[jss::errored] = std::to_string(totalRpc.sync.errored);
        totalRpcJson[jss::duration_us] = std::to_string(
            totalRpc.sync.duration.count());
        rpcobj[jss::total] = totalRpcJson;
    }

    Json::Value jqobj(Json::objectValue);
//totaljq表示所有作业。所有排队、开始、完成等。
    Jq totalJq("total");
    for (auto const& proc : jq_)
    {
        Json::Value j(Json::objectValue);
        {
            auto const sync =
                [&proc]() ->boost::optional<Counters::Jq::Sync> {
                std::lock_guard<std::mutex> lock(proc.second.mut);
                if (!proc.second.sync.queued &&
                    !proc.second.sync.started &&
                    !proc.second.sync.finished)
                {
                    return boost::none;
                }
                return proc.second.sync;
            }();
            if (!sync)
                continue;

            j[jss::queued] = std::to_string(sync->queued);
            totalJq.sync.queued += sync->queued;
            j[jss::started] = std::to_string(sync->started);
            totalJq.sync.started += sync->started;
            j[jss::finished] = std::to_string(sync->finished);
            totalJq.sync.finished += sync->finished;
            j[jss::queued_duration_us] = std::to_string(
                sync->queuedDuration.count());
            totalJq.sync.queuedDuration += sync->queuedDuration;
            j[jss::running_duration_us] = std::to_string(
                sync->runningDuration.count());
            totalJq.sync.runningDuration += sync->runningDuration;
        }
        jqobj[proc.second.label] = j;
    }

    if (totalJq.sync.queued)
    {
        Json::Value totalJqJson(Json::objectValue);
        totalJqJson[jss::queued] = std::to_string(totalJq.sync.queued);
        totalJqJson[jss::started] = std::to_string(totalJq.sync.started);
        totalJqJson[jss::finished] = std::to_string(totalJq.sync.finished);
        totalJqJson[jss::queued_duration_us] = std::to_string(
            totalJq.sync.queuedDuration.count());
        totalJqJson[jss::running_duration_us] = std::to_string(
            totalJq.sync.runningDuration.count());
        jqobj[jss::total] = totalJqJson;
    }

    Json::Value counters(Json::objectValue);
//善待报告工具，让它们期望RPC和JQ对象
//即使是空的。
    counters[jss::rpc] = rpcobj;
    counters[jss::job_queue] = jqobj;
    return counters;
}

Json::Value
PerfLogImp::Counters::currentJson() const
{
    auto const present = steady_clock::now();

    Json::Value jobsArray(Json::arrayValue);
    auto const jobs = [this]{
        std::lock_guard<std::mutex> lock(jobsMutex_);
        return jobs_;
    }();

    for (auto const& j : jobs)
    {
        if (j.first == jtINVALID)
            continue;
        Json::Value jobj(Json::objectValue);
        auto const e = jq_.find(j.first);
        if (e == jq_.end())
        {
            assert(false);
            continue;
        }
//标签是常量，在多线程之前创建，因此不需要锁定。
        jobj[jss::job] = e->second.label;
        jobj[jss::duration_us] = std::to_string(
            std::chrono::duration_cast<microseconds>(
                present - j.second).count());
        jobsArray.append(jobj);
    }

    Json::Value methodsArray(Json::arrayValue);
    std::vector<MethodStart> methods;
    {
        std::lock_guard<std::mutex> lock(methodsMutex_);
        methods.reserve(methods_.size());
        for (auto const& m : methods_)
            methods.push_back(m.second);
    }
    for (auto m : methods)
    {
        Json::Value methodobj(Json::objectValue);
        methodobj[jss::method] = m.first;
        methodobj[jss::duration_us] = std::to_string(
            std::chrono::duration_cast<microseconds>(
                present - m.second).count());
        methodsArray.append(methodobj);
    }

    Json::Value current(Json::objectValue);
    current[jss::jobs] = jobsArray;
    current[jss::methods] = methodsArray;
    return current;
}

//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

void
PerfLogImp::openLog()
{
    if (! setup_.perfLog.empty())
    {
        if (logFile_.is_open())
            logFile_.close();

        auto logDir = setup_.perfLog.parent_path();
        if (!boost::filesystem::is_directory(logDir))
        {
            boost::system::error_code ec;
            boost::filesystem::create_directories(logDir, ec);
            if (ec)
            {
                JLOG(j_.fatal()) << "Unable to create performance log "
                    "directory " << logDir << ": " << ec.message();
                signalStop_();
                return;
            }
        }

        logFile_.open(setup_.perfLog.c_str(), std::ios::out | std::ios::app);

        if (! logFile_)
        {
            JLOG(j_.fatal()) << "Unable to open performance log " <<
                setup_.perfLog << ".";
            signalStop_();
        }
    }
}

void
PerfLogImp::run()
{
    beast::setCurrentThreadName("perflog");
    lastLog_ = system_clock::now();

    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (stop_)
            {
                return;
            }
            if (rotate_)
            {
                openLog();
                rotate_ = false;
            }
            cond_.wait_until (lock, lastLog_ + setup_.logInterval);
        }
        report();
    }
}

void
PerfLogImp::report()
{
    if (! logFile_)
//如果日志文件_u不可写，请不要做进一步的工作。
        return;

    auto const present = system_clock::now();
    if (present < lastLog_ + setup_.logInterval)
        return;
    lastLog_ = present;

    Json::Value report(Json::objectValue);
    report[jss::time] = to_string(date::floor<microseconds>(present));
    report[jss::workers] = counters_.workers_;
    report[jss::hostid] = hostname_;
    report[jss::counters] = counters_.countersJson();
    auto cur = counters_.currentJson();
    report[jss::current_activities] = counters_.currentJson();

    logFile_ << Json::Compact{std::move(report)} << std::endl;
}

PerfLogImp::PerfLogImp(Setup const& setup,
    Stoppable& parent,
    beast::Journal journal,
    std::function<void()>&& signalStop)
        : Stoppable ("PerfLogImp", parent)
        , setup_ (setup)
        , j_ (journal)
        , signalStop_ (std::move (signalStop))
{
    openLog();
}

PerfLogImp::~PerfLogImp()
{
    onStop();
}

void
PerfLogImp::rpcStart(std::string const& method, std::uint64_t const requestId)
{
    auto counter = counters_.rpc_.find(method);
    if (counter == counters_.rpc_.end())
    {
        assert(false);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(counter->second.mut);
        ++counter->second.sync.started;
    }
    std::lock_guard<std::mutex> lock(counters_.methodsMutex_);
    counters_.methods_[requestId] = {
        counter->first.c_str(),
        steady_clock::now()
    };
}

void
PerfLogImp::rpcEnd(std::string const& method,
    std::uint64_t const requestId,
    bool finish)
{
    auto counter = counters_.rpc_.find(method);
    if (counter == counters_.rpc_.end())
    {
        assert(false);
        return;
    }
    steady_time_point startTime;
    {
        std::lock_guard<std::mutex> lock(counters_.methodsMutex_);
        auto const e = counters_.methods_.find(requestId);
        if (e != counters_.methods_.end())
        {
            startTime = e->second.second;
            counters_.methods_.erase(e);
        }
        else
        {
            assert(false);
        }
    }
    std::lock_guard<std::mutex> lock(counter->second.mut);
    if (finish)
        ++counter->second.sync.finished;
    else
        ++counter->second.sync.errored;
    counter->second.sync.duration +=
        std::chrono::duration_cast<microseconds>(
            steady_clock::now() - startTime);
}

void
PerfLogImp::jobQueue(JobType const type)
{
    auto counter = counters_.jq_.find(type);
    if (counter == counters_.jq_.end())
    {
        assert(false);
        return;
    }
    std::lock_guard<std::mutex> lock(counter->second.mut);
    ++counter->second.sync.queued;
}

void
PerfLogImp::jobStart(JobType const type,
    microseconds dur,
    steady_time_point startTime,
    int instance)
{
    auto counter = counters_.jq_.find(type);
    if (counter == counters_.jq_.end())
    {
        assert(false);
        return;
    }
    {
        std::lock_guard<std::mutex> lock(counter->second.mut);
        ++counter->second.sync.started;
        counter->second.sync.queuedDuration += dur;
    }
    std::lock_guard<std::mutex> lock(counters_.jobsMutex_);
    if (instance >= 0 && instance < counters_.jobs_.size())
        counters_.jobs_[instance] = {type, startTime};
}

void
PerfLogImp::jobFinish(JobType const type, microseconds dur,
    int instance)
{
    auto counter = counters_.jq_.find(type);
    if (counter == counters_.jq_.end())
    {
        assert(false);
        return;
    }
    {
        std::lock_guard<std::mutex> lock(counter->second.mut);
        ++counter->second.sync.finished;
        counter->second.sync.runningDuration += dur;
    }
    std::lock_guard<std::mutex> lock(counters_.jobsMutex_);
    if (instance >= 0 && instance < counters_.jobs_.size())
        counters_.jobs_[instance] = {jtINVALID, steady_time_point()};
}

void
PerfLogImp::resizeJobs(int const resize)
{
    std::lock_guard<std::mutex> lock(counters_.jobsMutex_);
    counters_.workers_ = resize;
    if (resize > counters_.jobs_.size())
        counters_.jobs_.resize(resize, {jtINVALID, steady_time_point()});
}


void
PerfLogImp::rotate()
{
    if (setup_.perfLog.empty())
        return;

    std::lock_guard<std::mutex> lock(mutex_);
    rotate_ = true;
    cond_.notify_one();
}

void
PerfLogImp::onStart()
{
    if (setup_.perfLog.size())
        thread_ = std::thread(&PerfLogImp::run, this);
}

void
PerfLogImp::onStop()
{
    if (thread_.joinable())
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
            cond_.notify_one();
        }
        thread_.join();
    }
    if (areChildrenStopped())
        stopped();
}

void
PerfLogImp::onChildrenStopped()
{
    onStop();
}

//————————————————————————————————————————————————————————————————————————————————————————————————————————————————

PerfLog::Setup
setup_PerfLog(Section const& section, boost::filesystem::path const& configDir)
{
    PerfLog::Setup setup;
    std::string perfLog;
    set(perfLog, "perf_log", section);
    if (perfLog.size())
    {
        setup.perfLog = boost::filesystem::path(perfLog);
        if (setup.perfLog.is_relative())
        {
            setup.perfLog = boost::filesystem::absolute(
                setup.perfLog, configDir);
        }
    }

    std::uint64_t logInterval;
    if (get_if_exists(section, "log_interval", logInterval))
        setup.logInterval = std::chrono::seconds(logInterval);
    return setup;
}

std::unique_ptr<PerfLog>
make_PerfLog(
    PerfLog::Setup const& setup,
    Stoppable& parent,
    beast::Journal journal,
    std::function<void()>&& signalStop)
{
    return std::make_unique<PerfLogImp>(
        setup, parent, journal, std::move(signalStop));
}

} //珀尔夫
} //涟漪
