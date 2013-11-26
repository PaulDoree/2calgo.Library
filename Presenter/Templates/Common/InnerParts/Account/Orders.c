const int MODE_TRADES = 0;
const int SELECT_BY_POS = 0;
const int SELECT_BY_TICKET = 1;

T GetPropertyValue<T>(Func<Position, T> getFromPosition, Func<PendingOrder, T> getFromPendingOrder)
{
	if (_currentOrder == null)
		return default(T);

	return GetPropertyValue<T>(_currentOrder, getFromPosition, getFromPendingOrder);
}

T GetPropertyValue<T>(object obj, Func<Position, T> getFromPosition, Func<PendingOrder, T> getFromPendingOrder)
{
	if (obj is Position)
		return getFromPosition((Position) obj);
	return getFromPendingOrder((PendingOrder) obj);
}

private Mq4Double GetTicket(object trade)
{
	return new Mq4Double(GetPropertyValue<int>(trade, _ => _.Id, _ => _.Id) + (int)1e8);
}

[Conditional("OrderTicket", "OrderPrint")]
Mq4Double OrderTicket()
{
	if (_currentOrder == null)
		return 0;

	return GetTicket(_currentOrder);
}

private int GetMagicNumber(string label)
{			
	var magicNumberStr = label;
	if (string.IsNullOrEmpty(magicNumberStr))
		return 0;

	var sharpIndex = magicNumberStr.IndexOf("#");
	if (sharpIndex != -1)
		magicNumberStr = magicNumberStr.Substring(0, sharpIndex);

	int magicNumber;
	if (int.TryParse(magicNumberStr, out magicNumber))
		return magicNumber;

	return 0;
}

private string GetComment(string label)
{
	if (string.IsNullOrEmpty(label))
		return string.Empty;

	var sharpIndex = label.IndexOf("#");
	if (sharpIndex == -1)
		return string.Empty;

	return label.Substring(sharpIndex, label.Length - sharpIndex);
}

private int GetMagicNumber(object order)
{	
	var label = GetPropertyValue<string>(order, _ => _.Label, _ => _.Label);
	return GetMagicNumber(label);
}

[Conditional("OrderMagicNumber", "OrderPrint")]
Mq4Double OrderMagicNumber()
{
	if (_currentOrder == null)
		return 0;

	return GetMagicNumber(_currentOrder);
}

[Conditional("OrderComment", "OrderPrint")]
Mq4String OrderComment()
{
	if (_currentOrder == null)
		return string.Empty;

	return GetPropertyValue<string>(_currentOrder, _ => _.Comment, _ => _.Comment);
}

[Conditional("OrdersTotal")]
Mq4Double OrdersTotal()
{
	return Positions.Count + PendingOrders.Count;
}

object _currentOrder;

[Conditional("OrderSelect")]
bool OrderSelect(int index, int select, int pool = MODE_TRADES)
{
	_currentOrder = null;

	var allOrders = Positions.OfType<object>()
							.Concat(PendingOrders.OfType<object>())
							.ToArray();

	switch (select)
	{
		case SELECT_BY_POS:	
			if (index < 0 || index >= allOrders.Length)
				return false;

			_currentOrder = allOrders[index];
			return true;
		case SELECT_BY_TICKET:
			_currentOrder = GetOrderByTicket(index);
			return _currentOrder != null;
	}	

	return false;
}

double GetLots(object order)
{
	var volume = GetPropertyValue<long>(order, _ => _.Volume,  _ => _.Volume);
	var symbolCode = GetPropertyValue<string>(order, _ => _.SymbolCode, _ => _.SymbolCode);
	var symbolObject = MarketData.GetSymbol(symbolCode);
	
	return symbolObject.ToLotsVolume(volume);
}

object GetOrderByTicket(int ticket)
{
	var allOrders = Positions.OfType<object>()
							.Concat(PendingOrders.OfType<object>())
							.ToArray();

	return allOrders.FirstOrDefault(_ => GetTicket(_) == ticket);
}

[Conditional("OrderLots", "OrderPrint")]
Mq4Double OrderLots()
{
	if (_currentOrder == null)
		return 0;

	return GetLots(_currentOrder);
}

[Conditional("OrderType", "OrderPrint")]
Mq4Double OrderType()
{
	if (_currentOrder == null)
		return 0;

	var position = _currentOrder as Position;
	if (position != null)
	{
		return position.TradeType == TradeType.Buy ? OP_BUY : OP_SELL;
	}
	var pendingOrder = (PendingOrder)_currentOrder;
	if (pendingOrder.OrderType == PendingOrderType.Limit)
		return pendingOrder.TradeType == TradeType.Buy ? OP_BUYLIMIT : OP_SELLLIMIT;
	return pendingOrder.TradeType == TradeType.Buy ? OP_BUYSTOP : OP_SELLSTOP;
}

[Conditional("OrderSymbol")]
Mq4String OrderSymbol()
{
	return GetPropertyValue<string>(_ => _.SymbolCode, _ => _.SymbolCode);
}

double GetOpenPrice(object order)
{
	return GetPropertyValue<double>(order, _ => _.EntryPrice, _ => _.TargetPrice);
}

[Conditional("OrderOpenPrice", "OrderPrint")]
Mq4Double OrderOpenPrice()
{
	if (_currentOrder == null)
		return 0;
	return GetOpenPrice(_currentOrder);
}

private double GetStopLoss(object order)
{
	var nullableValue = GetPropertyValue<double?>(order, _ => _.StopLoss, _ => _.StopLoss);
	return nullableValue ?? 0;
}

private double GetTakeProfit(object order)
{
	var nullableValue = GetPropertyValue<double?>(order, _ => _.TakeProfit, _ => _.TakeProfit);
	return nullableValue ?? 0;
}

[Conditional("OrderStopLoss", "OrderPrint")]
Mq4Double OrderStopLoss()
{	
	if (_currentOrder == null)
		return 0;
	return GetStopLoss(_currentOrder);
}

[Conditional("OrderTakeProfit", "OrderPrint")]
Mq4Double OrderTakeProfit()
{
	if (_currentOrder == null)
		return 0;
	return GetTakeProfit(_currentOrder);
}

[Conditional("OrderProfit", "OrderPrint")]
Mq4Double OrderProfit()
{
	var position = _currentOrder as Position;
	if (position == null)
		return 0;
	return position.NetProfit;
}

[Conditional("OrderOpenTime", "OrderPrint")]
Mq4Double OrderOpenTime()
{
	var position = _currentOrder as Position;
	if (position == null)
		return 0;

	return Mq4TimeSeries.ToInteger(position.EntryTime);
}

[Conditional("OrderExpiration", "OrderPrint")]
Mq4Double OrderExpiration()
{
	var pendingOrder = _currentOrder as PendingOrder;
	if (pendingOrder == null || pendingOrder.ExpirationTime == null)
		return 0;

	return Mq4TimeSeries.ToInteger(pendingOrder.ExpirationTime.Value);
}

[Conditional("OrderSwap", "OrderPrint")]
Mq4Double OrderSwap()
{
	var position = _currentOrder as Position;
	if (position == null)
		return 0;

	return position.Swap;
}

[Conditional("OrderCommission", "OrderPrint")]
Mq4Double OrderCommission()
{
	var position = _currentOrder as Position;
	if (position == null)
		return 0;

	return position.Commissions;
}

[Conditional("OrderPrint")]
private string OrderTypeString()
{
	switch(OrderType())
	{
		case OP_BUY:
			return "buy";
		case OP_SELL:
			return "sell";
		case OP_BUYLIMIT:
			return "buy limit";
		case OP_SELLLIMIT:
			return "sell limit";
		case OP_BUYSTOP:
			return "buy stop";
		case OP_SELLSTOP:
			return "sell stop";
	}
	return "0";
}

[Conditional("OrderPrint")]
void OrderPrint()
{
	var values = new string[]
	{
		OrderTicket().ToString(),
		Mq4TimeSeries.ToDateTime(OrderOpenTime()).ToString(),
		OrderTypeString(),
		OrderLots().ToString(),
		OrderOpenPrice().ToString(),
		OrderStopLoss().ToString(),
		OrderTakeProfit().ToString(),
		"0",
		"0",
		OrderCommission().ToString(),
		OrderSwap().ToString(),
		OrderProfit().ToString(),
		OrderComment(),
		OrderMagicNumber().ToString(),
		OrderExpiration() == 0 ? "0" : Mq4TimeSeries.ToDateTime(OrderExpiration()).ToString(),
	};
	Print(string.Join("; ", values));
}