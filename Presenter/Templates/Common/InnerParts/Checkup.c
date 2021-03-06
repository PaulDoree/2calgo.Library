[Conditional("IsConnected")]
Mq4Double IsConnected()
{
    return true;
}
	
private int _lastError;

[Conditional("GetLastError")]
Mq4Double GetLastError()
{
	return _lastError;
}

[Conditional("IsTesting", "Alert")]
Mq4Double IsTesting()
{
    return IsBacktesting;
}

[Conditional("IsOptimization")]
Mq4Double IsOptimization()
{
	return false;
}

[Conditional("IsStopped")]
Mq4Double IsStopped()
{
	return false;
}

[Conditional("UninitializeReason")]
Mq4Double UninitializeReason()
{
    return REASON_REMOVE;
}

[Conditional("IsTradeContextBusy")]
Mq4Double IsTradeContextBusy()
{
    return false;
}

[Conditional("IsExpertEnabled")]
Mq4Double IsExpertEnabled()
{
    return true;
}

[Conditional("IsDemo")]
Mq4Double IsDemo()
{
    return !Account.IsLive;
}

[Conditional("IsLibrariesAllowed")]
Mq4Double IsLibrariesAllowed()
{
    return true;
}

[Conditional("IsDllsAllowed")]
Mq4Double IsDllsAllowed()
{
	return false;
}

[Conditional("IsVisualMode")]
Mq4Double IsVisualMode()
{
    return false;
}

[Conditional("TerminalCompany")]
Mq4String TerminalCompany()
{
    return "Unknown";
}

