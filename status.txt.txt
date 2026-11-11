# Test Plan — Smart Study Room Occupancy Tracker

The program starts with 0 people and a maximum capacity of 5.

| Test | Action | Expected Result | Actual Result | Result |
|---|---|---|---|---|
| 1 | Add one person | Count becomes 1/5 | Count was 1/5 | PASS |
| 2 | Remove someone from an empty room | Count stays at 0 | Count stayed at 0 | PASS |
| 3 | Add five people | Room becomes full and LED turns ON | Room was full and LED turned ON | PASS |
| 4 | Try to add a sixth person | Entry is rejected | Entry was rejected | PASS |
| 5 | Change capacity to 3 | New capacity is saved | Capacity changed to 3 | PASS |
| 6 | Set capacity below current occupancy | Change is rejected | Change was rejected | PASS |
| 7 | Enter an invalid menu option | Error message is displayed | Error message was displayed | PASS |

## Test Notes

- All seven tests passed on the computer.
- The LED was simulated during testing.
- The physical LED still needs to be tested on the Raspberry Pi.
- Entry, exit, and capacity changes were saved to the CSV file.
