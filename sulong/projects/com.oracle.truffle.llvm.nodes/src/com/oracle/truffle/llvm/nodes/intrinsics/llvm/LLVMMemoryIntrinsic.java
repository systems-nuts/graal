/*
 * Copyright (c) 2016, 2019, Oracle and/or its affiliates.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 * conditions and the following disclaimer in the documentation and/or other materials provided
 * with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package com.oracle.truffle.llvm.nodes.intrinsics.llvm;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.dsl.Cached;
import com.oracle.truffle.api.dsl.NodeChild;
import com.oracle.truffle.api.dsl.Specialization;
import com.oracle.truffle.llvm.runtime.memory.LLVMMemSetNode;
import com.oracle.truffle.llvm.runtime.memory.LLVMMemory;
import com.oracle.truffle.llvm.runtime.memory.LLVMMemoryOpNode;
import com.oracle.truffle.llvm.runtime.nodes.api.LLVMExpressionNode;
import com.oracle.truffle.llvm.runtime.pointer.LLVMNativePointer;

public abstract class LLVMMemoryIntrinsic extends LLVMExpressionNode {

    @NodeChild(type = LLVMExpressionNode.class)
    public abstract static class LLVMMalloc extends LLVMMemoryIntrinsic {

        @Specialization
        protected LLVMNativePointer doVoid(int size,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            try {
                return memory.allocateMemory(size);
            } catch (OutOfMemoryError e) {
                CompilerDirectives.transferToInterpreter();
                return LLVMNativePointer.createNull();
            }
        }

        @Specialization
        protected LLVMNativePointer doVoid(long size,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            try {
                return memory.allocateMemory(size);
            } catch (OutOfMemoryError e) {
                CompilerDirectives.transferToInterpreter();
                return LLVMNativePointer.createNull();
            }
        }
    }

    @NodeChild(type = LLVMExpressionNode.class)
    @NodeChild(type = LLVMExpressionNode.class)
    public abstract static class LLVMCalloc extends LLVMMemoryIntrinsic {
        @Child private LLVMMemSetNode memSet;

        public LLVMCalloc(LLVMMemSetNode memSet) {
            this.memSet = memSet;
        }

        @Specialization
        protected LLVMNativePointer doVoid(int n, int size,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            try {
                long length = Math.multiplyExact(n, size);
                LLVMNativePointer address = memory.allocateMemory(length);
                memSet.executeWithTarget(address, (byte) 0, length);
                return address;
            } catch (OutOfMemoryError | ArithmeticException e) {
                CompilerDirectives.transferToInterpreter();
                return LLVMNativePointer.createNull();
            }
        }

        @Specialization
        protected LLVMNativePointer doVoid(long n, long size,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            try {
                long length = Math.multiplyExact(n, size);
                LLVMNativePointer address = memory.allocateMemory(length);
                memSet.executeWithTarget(address, (byte) 0, length);
                return address;
            } catch (OutOfMemoryError | ArithmeticException e) {
                CompilerDirectives.transferToInterpreter();
                return LLVMNativePointer.createNull();
            }
        }
    }

    @NodeChild(type = LLVMExpressionNode.class)
    @NodeChild(type = LLVMExpressionNode.class)
    public abstract static class LLVMRealloc extends LLVMMemoryIntrinsic {

        public abstract LLVMNativePointer executeWithTarget(LLVMNativePointer addr, Object size);

        @Specialization
        protected LLVMNativePointer doVoid(LLVMNativePointer addr, int size,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            return doVoid(addr, (long) size, memory);
        }

        @Specialization
        @SuppressWarnings("deprecation")
        protected LLVMNativePointer doVoid(LLVMNativePointer addr, long size,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            try {
                return memory.reallocateMemory(addr, size);
            } catch (OutOfMemoryError e) {
                CompilerDirectives.transferToInterpreter();
                return LLVMNativePointer.createNull();
            }
        }
    }

    @NodeChild(type = LLVMExpressionNode.class)
    public abstract static class LLVMFree extends LLVMMemoryIntrinsic implements LLVMMemoryOpNode {

        @Specialization
        protected Object doVoid(LLVMNativePointer address,
                        @Cached("getLLVMMemory()") LLVMMemory memory) {
            memory.free(address);
            return null;
        }
    }
}